#include "bsdf.h"
#include "math_util.h"

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


Vec3 FresnelMirrorBRDF::sample(const Vec3& incoming, const Vec3& normal,
                               scalar r1, scalar r2, scalar& p, scalar& reflectance) const
{
  p = 1.0;
  reflectance = fresnel_reflectance(normal, incoming, refraction_index::AIR, ri);
  Vec3 proj = incoming.projectOnto(normal);
  return proj * 2.0 - incoming;
}
