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
  : russian_roulette(true), rr_kill_prob(0.1), min_rr_depth(4), max_depth(10)
{
  assert( 0.0 <= rr_kill_prob && rr_kill_prob <= 1.0 );
}

PathTracerIntegrator::PathTracerIntegrator(const PathTracerIntegrator::Options& opt_)
  : rays_traced(0), primary_rays_traced(0), opt(opt_)
{
}

spectrum PathTracerIntegrator::_trace_ray(const Scene& scene, const Ray& ray,
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

  spectrum total(0);

  total += isect.emission();

  // direct lighting: randomly choose a light, and contribute
  // the light from that shape if appropriate
  scalar light_prob;
  const auto light = scene.sample_light(sampler.sample_1d(), light_prob);

  if (light_prob > 0)
  {
    LightSample ls = light->sample_emission(isect, sampler);
    if (ls && ls.p() > 0)
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
    auto mat_sample = isect.sample_bsdf(ray_dir_origin, sampler);

    scalar nl = fabs(mat_sample.direction.dot(isect.normal));// max<scalar>(brdf_dir.dot(isect.normal), 0);
    if (mat_sample.prob > 0 && nl > 0)
    {
      total += _trace_ray(scene, Ray{isect.position, mat_sample.direction}.nudge(),
                          sampler, depth + 1) *
        mat_sample.reflectance *
        spectrum{p_mult / mat_sample.prob * nl};
    }
  }

  return total;
}

spectrum PathTracerIntegrator::trace_ray(const Scene& scene, const Ray& r,
                                         Sampler& sampler) const
{
  return _trace_ray(scene, r, sampler, 1);
}



#endif
