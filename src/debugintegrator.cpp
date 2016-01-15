#include "debugintegrator.h"
#include "mat33.h"
#include "util/timer.h"
#include <unordered_map>
#include <chrono>

#ifdef FEATURE_DEBUG_TRACER

using std::make_pair;

const int NUM_DISTINCT_ID_COLORS = 20;
spectrum color_list[NUM_DISTINCT_ID_COLORS];

DebugIntegrator::DebugIntegrator(const DebugIntegrator::Options& opt_) : opt(opt_)
{
  for (int i = 0; i < NUM_DISTINCT_ID_COLORS; ++i)
    color_list[i] = spectrum::from_hsv(i * 18, 0.8, 1.0);
}

/*
void DebugIntegrator::render_rect(const Camera& cam, const Scene& scene,
                                  const Film::Rect& rect, uint samples_per_pixel) const
{
  UniformSampler sampler;

  auto& film = get_thread_film();

  for (uint y = 0; y < rect.height; ++y)
  {
    for (uint x = 0; x < rect.width; ++x)
    {
      auto px = x + rect.x;
      auto py = y + rect.y;
      for (auto d = 0u; d < samples_per_pixel; ++d)
      {
        PixelSample ps = cam.sample_pixel(film.width, film.height, px, py, sampler);

        spectrum s = trace_ray(ps.ray, scene, sampler);
        film.add_sample(ps, s);
      }
    }
  }
}
*/

spectrum dir_to_spectrum(const Vec3& dir)
{
   auto c = spectrum{dir.x, dir.y, dir.z};
   return 0.5 * (c + spectrum{1.0});
}

spectrum DebugIntegrator::trace_ray(const Scene& scene, const Ray& ray,
                                    Sampler& sampler) const
{
  if (opt.type == DI_TIME_INTERSECT)
  {
    Timer timer;
    scene.intersect(ray);
    return spectrum{static_cast<scalar>(timer.since())};
  }

  auto isect_opt = scene.intersect(ray);

  if (opt.type == DI_ISECT)
    return spectrum{scalar(isect_opt.is() ? 1.0 : 0.0)};

  if (opt.type == DI_DEPTH)
  {
    if (isect_opt.is())
      return spectrum(1.0 / max<scalar>((isect_opt.get().position - ray.position).norm(), 1.0));
    else
      return spectrum::zero;
  }
  if (opt.type == DI_OBJECT_ID)
  {
    if (!isect_opt.is())
      return spectrum::zero;
    
    auto isect = isect_opt.get();
    auto shape = isect.get_shape_for_id();
    {
      std::lock_guard<std::mutex> lg(color_mutex);
      if (color_map.find(shape) == color_map.end())
        color_map.insert(make_pair(shape, color_list[color_map.size() % NUM_DISTINCT_ID_COLORS]));
    }

    return color_map[shape];
  }

  if (opt.type == DI_NORMAL)
  {
    return isect_opt.is() ? dir_to_spectrum(isect_opt.get().normal) : spectrum::zero;
  }

  if (opt.type == DI_SPECULAR || opt.type == DI_SPECULAR_P)
  {
    if (!isect_opt.is())
      return spectrum::zero;

    auto ms = isect_opt.get().sample_bsdf(-ray.direction.normal(), sampler);
    if (opt.type == DI_SPECULAR)
      return dir_to_spectrum(ms.direction.normal());
    else // if (opt.type == DI_SPECULAR_P)
      return spectrum{ms.prob};
  }

  if (opt.type == DI_FIRST_ENV)
  {
    if (!isect_opt.is())
      return spectrum{0.3, 0.02, 0.05};

    auto isect = isect_opt.get();
    auto ms = isect.sample_bsdf(-ray.direction.normal(), sampler);

    if (ms.prob > 0)
    {
      return spectrum(scene.intersect(Ray{isect.position, ms.direction}.nudge()).is() ? 0.0 : 1.0);
    }
    return spectrum::zero;
  }

  return spectrum::zero;
}

#endif
