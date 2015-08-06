#include "env_light.h"
#include <cmath>

using std::tie;

spectrum EnvironmentalLight::emission(const Vec3& dir) const
{
  scalar theta, phi;
  tie(theta, phi) = (Mat33::y_to_z * dir).to_euler();

  return texture->at_coord(Vec2(theta / (2*PI), 1.0 - phi / M_PI));
}
