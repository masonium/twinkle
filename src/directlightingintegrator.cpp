
#include "directlightingintegrator.h"
#include "scheduler.h"
#include "sampler.h"
#include "util.h"
#include <iostream>
#include <algorithm>

using std::cerr;
using std::make_shared;
using std::transform;
using std::for_each;

DirectLightingIntegrator::Options::Options()
  : env_light_weight(.50), lighting_samples(16)
{

}

DirectLightingIntegrator::DirectLightingIntegrator(const DirectLightingIntegrator::Options& opt)
  : options(opt)
{
}

spectrum DirectLightingIntegrator::trace_ray(const Scene& scene, const Ray& ray,
                                             Sampler& shading_sampler) const
{
  auto isect_opt = scene.intersect(ray);
  if (!isect_opt.is())
    return scene.environment_light_emission(ray.direction.normal());

  auto isect = isect_opt.get();
  auto view_vector = -ray.direction.normal();
  const auto isect_normal = isect.normal;

  // For now, simple integrations
  auto lights = scene.lights();
  int num_lights = lights.size();

  spectrum total(0.0);

  int i = 0;
  int cum_samples = 0;
  int local_light_samples;

  local_light_samples = options.lighting_samples;

  for (auto light: lights)
  {
    int new_cum_samples = (local_light_samples * (i + 1)) / num_lights;
    int num_samples =  new_cum_samples - cum_samples;
    cum_samples = new_cum_samples;

    for (int s = 0; s < num_samples; ++s)
    {
      auto se = light->sample_emission(isect, shading_sampler);
      auto NL = std::max<scalar>(se.direction().dot(isect_normal), 0.0);

      if (NL > 0 && !se.is_occluded(scene))
        total += (NL / se.p()) * se.emission() * isect.reflectance(se.direction(), view_vector);
    }
    ++i;
  }

  return total / options.lighting_samples;
}
