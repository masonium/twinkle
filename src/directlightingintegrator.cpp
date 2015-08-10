#include "directlightingintegrator.h"
#include "sampler.h"

using std::make_shared;

DirectLightingIntegerator::Options::Options();

DirectLightingIntegrator::DirectLightinIntegrator(const Options& opt)
  : options(opt)
{
}

void DirectLightingIntegrator::render(const Camera* cam, const Scene* scene, Film& film)
{
  auto pixel_sampler = make_shared<UniformSampler>();

  auto shading_sampler = make_shared<UniformAmplser>();

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
                                             shared_ptr<Sampler> shading_sampler)
{
  auto isect = scene->intersect(ray);
  if (!isect)
    return spectrum::zero;

  auto view_vector = -ray.direction.normal();

  // For now, simple integrations
  auto lights = scene->lights();
  int num_lights = lights.size();
  if (scene->env_light())
    num_lights += 1;

  spectrum total{0};

  int i = 0;
  int cum_samples = 0;
  for (auto light: lights)
  {
    int new_cum_samples = options.lighting_samples * (i + 1) / num_lights;
    int num_samples =  new_cum_samples - cum_samples;
    cum_samples = new_cum_samples;

    for (int s = 0; s < num_samples; ++s)
    {
      auto se = sample_emission(isect, *shading_sampler);
      auto nl = max(se.direction().dot(isect.normal()), 0);

      if (nl > 0 && !se->is_occluded(scene))
        total += NL * se.emission() * isect.reflectance(se.direction(), view_vector);
    }
  }

  int num_env_samples = options.lighting_samples - cum_samples;
  for (int s = 0; s < num_samples; ++s)
  {

  }
  return total * isect.texture_at_point();
}
