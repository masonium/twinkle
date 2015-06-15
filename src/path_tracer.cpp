#include "path_tracer.h"
#include "util.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <memory>

using std::cerr;
using std::queue;
using std::mutex;
using std::lock_guard;
using std::make_shared;
using std::shared_ptr;
using std::thread;


PathTracerIntegrator::Options::Options()
  : russian_roulette(true), rr_kill_prob(0.1), min_rr_depth(4),
    max_depth(10), samples_per_pixel(16), num_threads(1)
{
  assert( 0.0 <= rr_kill_prob && rr_kill_prob <= 1.0 );
}


PathTracerIntegrator::PathTracerIntegrator(const PathTracerIntegrator::Options& opt_)
  : rays_traced(0), primary_rays_traced(0), opt(opt_)
{
}

void PathTracerIntegrator::render(const Camera* cam, const Scene* scene, Film& film)
{
  int num_threads = opt.num_threads;
  if (num_threads == 0)
  {
    num_threads = num_system_procs();
  }

  TaskQueue queue;

  if (num_threads == 1)
  {
    queue.emplace(Film::Rect{0, 0, film.width, film.height}, opt.samples_per_pixel);
    auto temp_film = make_shared<Film>(film.width, film.height, film.filter);
    render_thread(queue, cam, scene, temp_film);
    film.merge( *temp_film.get() );
    return;
  }

  const uint PER_SIDE = 2;
  const int SAMPLES_PER_TASK = 16;
  
  int samples_left = opt.samples_per_pixel;
  
  // Populate the task queue.
  while (samples_left > 0)
  {
    int task_samples = min(samples_left, SAMPLES_PER_TASK);
    samples_left -= SAMPLES_PER_TASK;
    for (uint i = 0; i < PER_SIDE; ++i)
    {
      uint x = film.width * i / PER_SIDE;
      uint w = film.width * (i+1) / PER_SIDE - film.width * i / PER_SIDE;
      
      for (uint j = 0; j < PER_SIDE; ++j)
      {
        uint y = film.height * j / PER_SIDE;
        uint h = film.height * (j+1) / PER_SIDE - film.height * j / PER_SIDE;

        queue.emplace(Film::Rect{x, y, w, h}, static_cast<uint>(task_samples));
      }
    }
  }

  // Create adn run the threads.
  vector<shared_ptr<Film>> films;
  vector<thread> threads;
  for (int i = 0; i < num_threads; ++i)
  {
    films.push_back(make_shared<Film>(film.width, film.height, film.filter));
    threads.push_back(thread(&PathTracerIntegrator::render_thread,
                             this, std::ref(queue), cam, scene, films[i]));
  }

  // Merge and join the results.
  for (auto& th: threads)
    th.join();

  for (const auto& f: films)
    film.merge(*f.get());
}

spectrum PathTracerIntegrator::trace_ray(const Scene* scene, const Ray& ray,
                                         shared_ptr<UniformSampler> sampler, int depth) const
{
  ++rays_traced;
  if (depth == 1)
    ++primary_rays_traced;
  
  Intersection isect = scene->intersect(ray);
  const Vec3 ray_dir_origin = -ray.direction.normal();
  if (!isect.valid())
    return environmental_lighting(-ray_dir_origin);
  if (isect.is_emissive())
    return isect.emission();

  // direct lighting: randomly choose a light or emissive shape, and contribute
  // the light from that shape if appropriate
  scalar light_prob;
  const Light* light = scene->sample_light(sampler->sample_1d(), light_prob);

  spectrum total{0};

  if (light_prob > 0)
  {
    Sample2D light_s = sampler->sample_2d();
    LightSample ls = light->sample_emission(isect, light_s[0], light_s[1]);

    if (ls)
    {
      if (!ls.is_occluded(scene))
      {
        scalar NL = max<scalar>(ls.direction().dot(isect.normal), 0.0);
        scalar ca = isect.reflectance(ls.direction(), ray_dir_origin);

        auto light_contrib = ls.emission() * spectrum{NL * ca / light_prob};
        total += light_contrib;
      }
    }
  }

  // Decide whether or not to continue trace and, if so, what the multiplier
  // should be.
  bool continue_trace = true;
  scalar p_mult = 1.0;
  if (opt.max_depth > 0 && depth >= opt.max_depth)
    continue_trace = false;
  else if (opt.russian_roulette && depth >= opt.min_rr_depth)
  {
    if (sampler->sample_1d() < opt.rr_kill_prob)
    {
      continue_trace = false;
    }
    else
    {
      p_mult /= (1 - opt.rr_kill_prob);
    }
  }

  if (continue_trace)
  {
    scalar brdf_p = 0;
    auto brdf_u = sampler->sample_2d();
    scalar brdf_reflectance;
    Vec3 brdf_dir = isect.sample_bsdf(ray_dir_origin, brdf_u,
                                      brdf_p, brdf_reflectance);

    scalar nl = fabs(brdf_dir.dot(isect.normal));
    if (brdf_p > 0)
    {
      total += trace_ray(scene, Ray{isect.position, brdf_dir}.nudge(),
                         sampler, depth + 1) *
        spectrum{p_mult / brdf_p * nl * brdf_reflectance};
    }
  }

  return total * isect.texture_at_point();
}

void PathTracerIntegrator::render_thread(TaskQueue& queue, const Camera* cam,
                                         const Scene* scene, shared_ptr<Film> film) const
{
  auto sampler = make_shared<UniformSampler>();
  
  while (true)
  {
    Task task;
    
    {
      lock_guard<mutex> lock(render_queue_mutex);
      if (queue.empty())
        break;
      task = queue.front();
      queue.pop();
    }

    if (task.samples_per_pixel == 0)
      break;

    for (uint x = 0; x < task.rect.width; ++x)
    {
      for (uint y = 0; y < task.rect.height; ++y)
      {
        for (uint d = 0; d < task.samples_per_pixel; ++d)
        {
          auto sample = sampler->sample_5d();
          
          int px = x + task.rect.x;
          int py = y + task.rect.y;
          
          PixelSample ps = cam->sample_pixel(*film, px, py, sample);
          
          spectrum s = trace_ray(scene, ps.ray, sampler, 1);

          film->add_sample(ps, s);
        }
      }
    }
  }
}

