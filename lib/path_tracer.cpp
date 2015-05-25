#include "integrator.h"

PathTracerIntegrator::PathTracerIntegrator()
  : sampler(unique_ptr<UniformSampler>(new UniformSampler())), samples_per_pixel(16), 
    russian_roulette(true), rr_kill_prob(0.2), max_depth(10)
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
        spectrum s = trace_ray(scene, r, 1);
        film->add_sample(x, y, s);
      }
    }
  }
}

spectrum PathTracerIntegrator::trace_ray(Scene* scene, const Ray& ray, int depth)
{
  Intersection isect = scene->intersect(ray);
  if (!isect.valid())
    return spectrum::zero;
  if (isect.shape->is_emissive())
    return isect.shape->emission() * isect.texture_at_point();
  
  // direct lighting: randomly choose a light or emissive shape, and contribute
  // the light from that shape if appropriate
  scalar light_prob;
  auto em = scene->sample_emissive(sampler->sample_1d(), &light_prob);

  spectrum total{0};

  const Vec3 ray_dir_normal = -ray.direction.normal();

  Sample2D shadow_ray_sample = sampler->sample_2d();
  const Vec3 shadow_ray_dir = em->sample_shadow_ray_dir(isect, shadow_ray_sample.u[0],
    shadow_ray_sample.u[1]);
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
  scalar p_mult = 1.0;
  if (max_depth > 0 && depth >= max_depth)
    continue_trace = false;

  
  else if (russian_roulette)
  {
    if (sampler->sample_1d() < rr_kill_prob)
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
    scalar brdf_p = 0;
    auto brdf_u = sampler->sample_2d();
    Vec3 brdf_dir = isect.sample_brdf(ray_dir_normal, brdf_u.u[0], brdf_u.u[1], brdf_p);
    scalar ca = isect.shape->brdf->reflectance( brdf_dir, ray_dir_normal, isect.normal );
    if (brdf_p > 0)
      total += trace_ray( scene, Ray{isect.position, brdf_dir}.nudge(), depth + 1 ) * 
        spectrum{p_mult * brdf_dir.dot(isect.normal) * ca / brdf_p};
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
    return spectrum::zero;
}
