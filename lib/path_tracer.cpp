#include "integrator.h"

PathTracerIntegrator::PathTracerIntegrator()
  : randr(0.0, 1.0), samples_per_pixel(16), russian_roulette(true), 
rr_kill_prob(0.2), max_depth(10)
{
  assert(0.0 <= rr_kill_prob && rr_kill_prob <= 1.0); 
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

spectrum PathTracerIntegrator::trace_ray(Scene* scene, const Ray& ray, int depth)
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

  const Vec3 ray_dir_normal = -ray.direction.normal();

  const Vec3 shadow_ray_dir = em->sample_shadow_ray_dir(isect, randr(reng), randr(reng));
  const scalar NL = shadow_ray_dir.dot(isect.normal);
  
  if (NL > 0)
  {
    Ray shadow_ray{ isect.position, shadow_ray_dir.normal() };
    scalar ca = isect.shape->brdf->reflectance( shadow_ray.direction,
                                                ray_dir_normal, isect.normal );
    total += trace_shadow_ray( scene, em, shadow_ray.nudge(0.0005) ) * 
      spectrum{NL * ca / light_prob};
  }


  // Decide whether or not to continue trace and, if so, what the multiplier
  // should be.
  bool continue_trace = true;
  double p_mult = 1.0;
  if (max_depth > 0 && depth >= max_depth)
    continue_trace = false;
  else if (russian_roulette)
  {
    if (randr(reng) < rr_kill_prob)
    {
      continue_trace = false;
    }
    else
    {
      p_mult /= (1 - rr_kill_prob);
    }
  }

  if (continue_trace)
  {
    Vec3 brdf_dir = isect.sample_brdf(ray_dir_normal, randr(reng), randr(reng));
    total += trace_ray( scene, Ray{isect.position, brdf_dir}.nudge() ) * p_mult;
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
