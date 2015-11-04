#include "env_light.h"
#include <cmath>

using std::tie;

spectrum EnvironmentLight::emission(const Vec3& dir) const
{
  scalar theta, phi;
  tie(theta, phi) = dir.to_euler();

  return texture->at_coord(Vec2(theta / (2*PI), phi / M_PI));
}
