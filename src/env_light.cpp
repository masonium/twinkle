#include "env_light.h"

using std::tie;

spectrum EnvironmentalLight::emission(const Vec3& dir) const
{
  scalar theta, phi;
  tie(theta, phi) = dir.to_euler();

  return texture->at_coord(theta / 2*PI, phi / PI);
}
