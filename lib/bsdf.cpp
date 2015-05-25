#include "bsdf.h"

Vec3 Diffuse::sample(const Vec3& incoming, const Vec3& normal,
                     scalar r1, scalar r2, scalar& p,
                     scalar& reflectance) const
{
  // cosine-weighted sampling
  scalar theta = 2 * PI * r1;
  scalar radius = sqrt(r2);

  scalar x = radius * cos(theta), y = radius * sin(theta);

  scalar z{sqrt(max(scalar(0.0), 1 - x*x - y*y))};
  p = z / PI;
  reflectance = r;

  return Vec3{x, y, z}.rotateMatch(Vec3::z_axis, normal);
}
