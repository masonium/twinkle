#include "bsdf.h"

Vec3 Diffuse::sample(const Vec3& incoming, const Vec3& normal,
                     scalar r1, scalar r2) const
{
  // Assuming the normal vector is the north pole on the hemisphere, theta is
  // the longitude and phi is the latitude;
  scalar theta = 2 * PI * r1;
  scalar phi = 2 * asin(sqrt(r2 * 0.5));

  return Vec3::from_euler(theta, phi);
}
