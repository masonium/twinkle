
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

  if (scene.env_light())
  {
    if (num_lights > 0)
    {
      if (options.env_light_weight > 0)
        local_light_samples = (1 - min<scalar>(options.env_light_weight, 1.0)) * options.lighting_samples;
      else
        local_light_samples = options.lighting_samples * static_cast<scalar>(num_lights) / (num_lights + 1);
    }
    else
      local_light_samples = 0;
  }
  else
  {
    local_light_samples = options.lighting_samples;
  }

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

  int num_env_samples = options.lighting_samples - local_light_samples;
  auto normal_rotation = Mat33::rotate_match(Vec3::z_axis, isect_normal);
  Vec3 new_pos = isect.position + isect_normal * 0.0001;
  for (int s = 0; s < num_env_samples; ++s)
  {
    scalar l_p;
    Vec3 l_dir = normal_rotation * cosine_weighted_hemisphere_sample(shading_sampler.sample_2d(), l_p);
    auto refl = isect.reflectance(l_dir, view_vector);

    auto light_ray = Ray{new_pos, l_dir};
    if (!scene.intersect(light_ray).is())
      total += (l_dir.dot(isect_normal) / l_p) * refl * scene.environment_light_emission(l_dir);
  }

  return total / options.lighting_samples;
}
