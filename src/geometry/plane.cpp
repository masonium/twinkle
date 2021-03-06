#include <sstream>
#include "plane.h"

Plane::Plane(const Vec3& normal, scalar offset) : N(normal), d(offset)
{
  scalar n = N.norm();
  assert(n > 0);
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

scalar_fp Plane::intersect(const Ray& r) const
{
  scalar s = N.dot(r.direction);
  if (fabs(s) < 0.0001)
    return sfp_none;
  return scalar_fp{-(d + N.dot(r.position)) / s};
}

Vec3 Plane::normal(SubGeo geo, const Vec3& point) const
{
  return N;
}

void Plane::texture_coord(SubGeo geo, const Vec3& pos, const Vec3& normal,
                          scalar& u, scalar& v) const
{
  u = pos.dot(U);
  v = pos.dot(V);
}

std::string Plane::to_string() const
{
  std::ostringstream s;
  s << "Plane(" << N << ", " << d << ")";
  return s.str();
}
