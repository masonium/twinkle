#include "integrator.h"

PathTracerIntegrator::PathTracerIntegrator()
  : randr(0.0, 1.0), samples_per_pixel(1), russian_roulette(true), max_depth(10)
{
  
}

void PathTracerIntegrator::render(Camera* cam, Scene* scene, Film* film)
{
  for (uint x = 0; x < film->width; ++x)
  {
    for (uint y = 0; y < film->height; ++y)
    {
      for (uint d = 0; d < samples_per_pixel; ++d)
      {
        Ray r = cam->sample_pixel_ray(film, x, y);
        spectrum s = trace_ray(scene, r);
        film->add_sample(x, y, s);
      }
    }
  }
}

spectrum PathTracerIntegrator::trace_ray(Scene* scene, const Ray& ray)
{
  Intersection isect = scene->intersect(ray);
  if (!isect.valid())
    return spectrum_zero;
  if (isect.shape->is_emissive())
    return isect.shape->emission() * isect.texture_at_point();
  
  // direct lighting: randomly choose a light or emissive shape, and contribute
  // the light from that shape if appropriate
  scalar light_prob;
  auto em = scene->sample_emissive(randr(reng), &light_prob);

  spectrum total{0};

  const Vec3 ray_dir_normal = ray.direction.normal();

  const Vec3 shadow_ray_dir = em->sample_shadow_ray_dir(isect, randr(reng), randr(reng));
  const scalar NL = shadow_ray_dir.dot(isect.normal);
  
  if (NL > 0)
  {
    Ray shadow_ray{ isect.position, shadow_ray_dir.normal() };
    scalar ca = isect.shape->brdf->reflectance( shadow_ray.direction,
                                                -ray_dir_normal, isect.normal );
    total += trace_shadow_ray( scene, em, shadow_ray.nudge() ) * spectrum{NL * ca / light_prob};
  }

  return total * isect.texture_at_point();
}

spectrum PathTracerIntegrator::trace_shadow_ray(Scene* scene, PossibleEmissive const* em,
                                                const Ray& ray)
{
  auto isect = scene->shadow_intersect(ray);
  if (isect.valid() && isect.shape == em)
    return em->emission();
  else 
    return spectrum_zero;
}
