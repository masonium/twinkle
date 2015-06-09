#include "plane.h"

Plane::Plane(const Vec3& normal, scalar offset) : N(normal), d(offset)
{
  scalar n = N.norm();
  d /= n;
  N /= n;

  U = Vec3::y_axis.cross(N);
  if (U.norm2() < 0.000001)
  {
    U = Vec3::z_axis.cross(N);
  }
  U = U.normal();
  V = N.cross(U).normal();
}

scalar Plane::intersect(const Ray& r) const
{
  scalar s = N.dot(r.direction);
  if (fabs(s) < 0.0001)
    return -1;
  return -(d + N.dot(r.position)) / s;
}

Vec3 Plane::normal(const Vec3& point) const
{
  return N;
}

Vec3 Plane::sample_shadow_ray_dir(const Intersection& isect,
                                  scalar r1, scalar r2) const
{
  return Vec3::zero;
}

void Plane::texture_coord(const Vec3& pos, const Vec3& normal,
                          scalar& u, scalar& v, Vec3& dpdu, Vec3& dpdv) const
{
  u = pos.dot(U);
  v = pos.dot(V);
  dpdu = U;
  dpdv = V;
}
