#include "bsdf.h"

Vec3 Diffuse::sample(const Vec3& incoming, const Vec3& normal,
                     scalar r1, scalar r2, scalar& p) const
{
  // Assuming the normal vector is the north pole on the hemisphere, theta is
  // the longitude and phi is the latitude;
  scalar theta = 2 * PI * r1;
  scalar phi = 2 * asin(sqrt(r2 * 0.5));

  p = 1.0 / (2 * PI);

  return Vec3::from_euler(theta, phi).rotateMatch(Vec3::z_axis, normal);
}
