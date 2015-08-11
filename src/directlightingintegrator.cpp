#include "directlightingintegrator.h"
#include "sampler.h"

using std::make_shared;

DirectLightingIntegrator::Options::Options()
  : samples_per_pixel(4), lighting_samples(16), env_light_weight(50)
{

}

DirectLightingIntegrator::DirectLightingIntegrator(const DirectLightingIntegrator::Options& opt)
  : options(opt)
{
}

void DirectLightingIntegrator::render(const Camera* cam, const Scene* scene, Film& film)
{
  auto pixel_sampler = make_shared<UniformSampler>();

  auto shading_sampler = make_shared<UniformSampler>();

  for (auto y = 0u; y < film.height; ++y)
  {
    for (auto x = 0u; x < film.width; ++x)
    {
      for (auto d = 0u; d < options.samples_per_pixel; ++d)
      {
        auto ps = cam->sample_pixel(film, x, y, *pixel_sampler);
        film.add_sample(ps, trace_ray(ps.ray, scene, shading_sampler));
      }
    }
  }
}

spectrum DirectLightingIntegrator::trace_ray(const Ray& ray, const Scene* scene,
                                             shared_ptr<Sampler> shading_sampler) const
{
  auto isect = scene->intersect(ray);
  if (!isect)
    return scene->environment_light_emission(ray.direction.normal());

  auto view_vector = -ray.direction.normal();
  const auto isect_normal = isect.normal;

  // For now, simple integrations
  auto lights = scene->lights();
  int num_lights = lights.size();

  spectrum total{0};

  int i = 0;
  int cum_samples = 0;
  int local_light_samples;

  if (scene->env_light())
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
      auto se = light->sample_emission(isect, *shading_sampler);
      auto NL = std::max<scalar>(se.direction().dot(isect_normal), 0.0);

      if (NL > 0 && !se.is_occluded(scene))
        total += NL * se.emission() * isect.reflectance(se.direction(), view_vector);
    }
    ++i;
  }

  int num_env_samples = options.lighting_samples - local_light_samples;
  auto normal_rotation = Mat33::rotate_match(Vec3::z_axis, isect_normal);
  for (int s = 0; s < num_env_samples; ++s)
  {
    Vec3 l_dir = normal_rotation * cosine_weighted_hemisphere_sample(shading_sampler->sample_2d());
    scalar refl = isect.reflectance(l_dir, view_vector);

    auto light_ray = Ray{isect.position, l_dir}.nudge();
    if (!scene->intersect(light_ray))
      total += l_dir.dot(isect_normal) * refl * scene->environment_light_emission(l_dir);
  }

  return total * isect.texture_at_point() / options.lighting_samples;
}
