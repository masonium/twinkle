#include "dintegrator.h"
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

void DebugIntegrator::render(const Camera& cam, const Scene& scene, Film& film)
{
  grid_render(*this, cam, scene, film, 1, 4, opt.samples_per_pixel);
}

void DebugIntegrator::render_rect(const Camera& cam, const Scene& scene, Film& film,
                                  const Film::Rect& rect, uint samples_per_pixel) const
{
  ShapeColorMap scm;
  UniformSampler sampler;

  for (uint y = 0; y < rect.height; ++y)
  {
    for (uint x = 0; x < rect.width; ++x)
    {
      auto px = x + rect.x;
      auto py = y + rect.y;
      for (auto d = 0u; d < samples_per_pixel; ++d)
      {
        PixelSample ps = cam.sample_pixel(film.width, film.height, px, py, sampler);

        spectrum s = trace_ray(ps.ray, scene, scm, sampler);
        film.add_sample(ps, s);
      }
    }
  }
}

spectrum dir_to_spectrum(const Vec3& dir)
{
   auto c = spectrum{dir.x, dir.y, dir.z};
   return 0.5 * (c + spectrum{1.0});
}

spectrum DebugIntegrator::trace_ray(const Ray& ray, const Scene& scene, 
                                    ShapeColorMap& scm, Sampler& sampler) const
{
  if (opt.type == DI_TIME_INTERSECT)
  {
    Timer timer;
    scene.intersect(ray);
    return spectrum{timer.since()};
  }

  auto isect_opt = scene.intersect(ray);

  if (opt.type == DI_ISECT)
    return spectrum{scalar(isect_opt.is() ? 1.0 : 0.0)};

  if (opt.type == DI_DEPTH)
  {
    if (isect_opt.is())
      return spectrum(1.0 / max<scalar>((isect_opt.get().position - ray.position).norm(), 1.0));
    else
      return spectrum{0.0};
  }
  if (opt.type == DI_OBJECT_ID)
  {
    if (!isect_opt.is())
      return spectrum::zero;
    
    auto isect = isect_opt.get();
    auto shape = isect.get_shape_for_id();
    if (scm.find(shape) == scm.end())
      scm.insert(make_pair(shape, color_list[scm.size() % NUM_DISTINCT_ID_COLORS]));

    return scm[shape];
  }

  if (opt.type == DI_NORMAL)
  {
    return isect_opt.is() ? dir_to_spectrum(isect_opt.get().normal) : spectrum::zero;
  }

  if (opt.type == DI_SPECULAR)
  {
    if (!isect_opt.is())
      return spectrum::zero;

    scalar brdf_p, reflectance;
    ConstSampler s{1.0, 0.0};
    auto dir = isect_opt.get().sample_bsdf(-ray.direction.normal(), s, brdf_p, reflectance);
    return dir_to_spectrum(dir);
  }

  if (opt.type == DI_FIRST_ENV)
  {
    if (!isect_opt.is())
      return spectrum{0.3, 0.02, 0.05};

    auto isect = isect_opt.get();
    scalar brdf_p = 0, brdf_r = 0;
    auto brdf_dir = isect.sample_bsdf(-ray.direction.normal(), sampler, brdf_p, brdf_r);

    if (brdf_p > 0)
    {
      return spectrum(scene.intersect(Ray{isect.position, brdf_dir}.nudge()).is() ? 0.0 : 1.0);
    }
    return spectrum{0.0};
  }

  return spectrum::zero;
}

#endif
