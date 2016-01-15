#include "bdptracer.h"

#ifdef FEATURE_BIDIRECTIONAL

BidirectionalPathTracer::BidirectionalPathTracer(const BDPTOptions& opt_)
  : opt(opt_)
{
}

spectrum BidirectionalPathTracer::trace_ray(const Scene& scene, const Ray& ray, Sampler& s1, Sampler& s2) const
{
  auto eye_path = construct_eye_path(ray, scene, s1);

  // Don't support light vertices connecting directly with the eye.
  if (eye_path.trivial())
    return eye_path.trivial_emission(scene);

  auto light_path = construct_light_path(scene, s2);


  return spectrum::zero;
}

BidirectionalPathTracer::EyePath BidirectionalPathTracer::construct_eye_path(
  const Ray& eye_ray, const Scene& scene, Sampler& sampler) const
{
  EyePath path(eye_ray);

  auto ray = eye_ray;

  construct_path(scene, ray, 1.0, opt.eye_path_max_length, sampler, path);
  return path;
}

BidirectionalPathTracer::LightPath BidirectionalPathTracer::construct_light_path(
  const Scene& scene, Sampler& sampler) const
{
  LightPath path;
  auto emission_sample = scene.sample_emission(sampler);
  path.add_light(emission_sample);

  auto ray = emission_sample.ray.nudge();

  construct_path(scene, ray, emission_sample.ray_prob,
                 opt.light_path_max_length, sampler, path);
  return path;
}


void BidirectionalPathTracer::construct_path(
  const Scene& scene, const Ray& start_ray, scalar init_p, size_t max_length,
  Sampler& sampler, Path& path) const
{
  // dir_p is the probability of hitting the next vertex (i.e. the probability
  // of choosing the direction that hits that vertex). The first vertex is
  // completely determined by our light sample.
  scalar dir_p = init_p;

  Ray ray = start_ray;
  for (auto i = 0u; i < max_length; ++i)
  {
    // Interesct with the scene to find the next vertex in the path.
    auto isect_opt = scene.intersect(ray);

    // Stop if we hit nothing, or something resembling a light.
    if (!isect_opt.is())
      break;

    const auto& isect = isect_opt.get();

    if (isect.is_emissive())
      break;

    // Stop probabilistic based on russian roulette.
    if (opt.rr_min_length > 0 && i >= opt.rr_min_length)
    {
      if (sampler.sample_1d() < opt.rr_terminate_p)
      {
        break;
      }
      else
      {
        dir_p *= (1 - opt.rr_terminate_p);
      }
    }

    // Sample to find the next point.
    auto mat_sample = isect.sample_bsdf(-ray.direction.normal(), sampler);
    path.add_vertex(isect, mat_sample.reflectance, mat_sample.prob);

    // The proability of this new ray is equal to the probabilty of the next ray.
    dir_p = mat_sample.prob;
    if (mat_sample.prob == 0)
      break;

    ray = Ray(isect.position, mat_sample.direction).nudge();
  }
}

spectrum BidirectionalPathTracer::EyePath::trivial_emission(const Scene& scene) const
{
  return scene.environment_light_emission(eye.ray.direction);
}


#endif
