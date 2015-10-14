#include "path_tracer.h"
#include "util.h"
#include "scheduler.h"
#include "grid_tasks.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <memory>

#ifdef FEATURE_PATH_TRACER

using std::cerr;
using std::queue;
using std::mutex;
using std::lock_guard;
using std::make_shared;
using std::shared_ptr;
using std::unique_ptr;

PathTracerIntegrator::Options::Options()
  : russian_roulette(true), rr_kill_prob(0.1), min_rr_depth(4),
    max_depth(10), samples_per_pixel(16), num_threads(0)
{
  assert( 0.0 <= rr_kill_prob && rr_kill_prob <= 1.0 );
}

PathTracerIntegrator::PathTracerIntegrator(const PathTracerIntegrator::Options& opt_)
  : rays_traced(0), primary_rays_traced(0), opt(opt_)
{
}

void PathTracerIntegrator::render(const Camera& cam, const Scene& scene, 
                                  Scheduler& scheduler, Film& film)
{
  grid_render(*this, cam, scene, film,
              scheduler, 4, opt.samples_per_pixel);
}

spectrum PathTracerIntegrator::trace_ray(const Scene& scene, const Ray& ray,
                                         Sampler& sampler, int depth) const
{
  ++rays_traced;
  if (depth == 1)
    ++primary_rays_traced;
  
  auto isect_opt = scene.intersect(ray);
  const Vec3 ray_dir_origin = -ray.direction.normal();
  if (!isect_opt.is())
    return scene.environment_light_emission(-ray_dir_origin);

  auto isect = isect_opt.get();
  if (isect.is_emissive())
    return isect.emission();

  // direct lighting: randomly choose a light or emissive shape, and contribute
  // the light from that shape if appropriate
  scalar light_prob;
  const Light* light = scene.sample_light(sampler.sample_1d(), light_prob);

  spectrum total(0);

  if (light_prob > 0)
  {
    LightSample ls = light->sample_emission(isect, sampler);
    if (ls)
    {
      if (!ls.is_occluded(scene))
      {
        scalar NL = max<scalar>(ls.direction().dot(isect.normal), 0.0);
        auto ca = isect.reflectance(ls.direction(), ray_dir_origin);

        auto light_contrib = ls.emission() * ca * spectrum{NL / (light_prob * ls.p())};
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
    if (sampler.sample_1d() < opt.rr_kill_prob)
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
    spectrum brdf_reflectance;
    Vec3 brdf_dir = isect.sample_bsdf(ray_dir_origin, sampler,
                                      brdf_p, brdf_reflectance);

    scalar nl = fabs(brdf_dir.dot(isect.normal));// max<scalar>(brdf_dir.dot(isect.normal), 0);
    if (brdf_p > 0 && nl > 0)
    {
      total += trace_ray(scene, Ray{isect.position, brdf_dir}.nudge(),
                         sampler, depth + 1) *
        brdf_reflectance *
        spectrum{p_mult / brdf_p * nl};
    }
  }

  return total;
}

void PathTracerIntegrator::render_rect(const Camera& cam, const Scene& scene,
                                       const Film::Rect& rect,
                                       uint samples_per_pixel) const
{
  auto sampler = UniformSampler{};

  auto& film = get_thread_film();
  
  for (uint x = 0; x < rect.width; ++x)
  {
    for (uint y = 0; y < rect.height; ++y)
    {
      const int px = x + rect.x;
      const int py = y + rect.y;
          
      for (uint d = 0; d < samples_per_pixel; ++d)
      {
//        std::cerr << "Printing width: " << d << " " << film.width << "\n";
        PixelSample ps = cam.sample_pixel(film.width, film.height, px, py, sampler);
          
        spectrum s = trace_ray(scene, ps.ray, sampler, 1);
//        std::cerr << "Printing width: " << d << " " << film.width << "\n";
        film.add_sample(ps, s);
      }
    }
  }
}

#endif
