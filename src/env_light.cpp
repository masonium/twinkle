#include "env_light.h"
#include <cmath>

using std::tie;

spectrum EnvironmentLight::emission(const Vec3& dir) const
{
  scalar theta, phi;
  tie(theta, phi) = dir.to_euler();

  return texture->at_coord(Vec2(theta / (2*PI), phi / M_PI));
}

LightSample EnvironmentLight::sample_emission(const Intersection& isect, Sampler& sampler) const
{
  // cosine-weighted sample on the normal direction
  scalar p;
  auto local_d = cosine_weighted_hemisphere_sample(sampler.sample_2d(), p);

  auto d = local_d.rotateMatch(Vec3::z_axis, isect.normal);

  auto emit = emission(d);

  return LightSample(emit, p, Ray{isect.position, d}.nudge(0.0005));
}
