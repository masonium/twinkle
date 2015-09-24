
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
  : env_light_weight(.50), samples_per_pixel(4), lighting_samples(16), num_threads(1), subdivision(16)
{

}

DirectLightingIntegrator::DirectLightingIntegrator(const DirectLightingIntegrator::Options& opt)
  : options(opt)
{
}

void DirectLightingIntegrator::render(const Camera& cam, const Scene& scene, Film& film)
{
  grid_render(*this, cam, scene, film, options.num_threads,
    options.subdivision, options.samples_per_pixel);
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

  spectrum total{0};

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
    scalar refl = isect.reflectance(l_dir, view_vector);

    auto light_ray = Ray{new_pos, l_dir};
    if (!scene.intersect(light_ray).is())
      total += l_dir.dot(isect_normal) * (refl / l_p) * scene.environment_light_emission(l_dir);
  }

  return total * isect.texture_at_point() / options.lighting_samples;
}

void DirectLightingIntegrator::render_rect(
  const Camera& cam, const Scene& scene, Film& film,
  const Film::Rect& render_rect, uint samples_per_pixel) const
{
  auto sampler = UniformSampler{};

  for (uint x = 0; x < render_rect.width; ++x)
  {
    for (uint y = 0; y < render_rect.height; ++y)
    {
      for (uint d = 0; d < samples_per_pixel; ++d)
      {
        int px = x + render_rect.x;
        int py = y + render_rect.y;

        PixelSample ps = cam.sample_pixel(film.width, film.height, px, py, sampler);

        spectrum s = trace_ray(scene, ps.ray, sampler);

        film.add_sample(ps, s);
      }
    }
  }
}
/*
DirectLightingIntegrator::RenderTask::RenderTask(
  const DirectLightingIntegrator& pit, const RenderInfo& ri_,
  vector<Film>& films_,  const Film::Rect& rect_, uint spp_) :
  owner(pit), ri(ri_), films(films_), rect(rect_), spp(spp_)
{
}

void DirectLightingIntegrator::RenderTask::run(uint worker_id)
{

}
*/
