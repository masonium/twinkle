#include "sphere.h"

scalar Sphere::intersect(const Ray& r) const
{
  const Vec3 to = r.position - position;

  const scalar a = r.direction.norm2();
  const scalar b = 2.0 * r.direction.dot(to);
  const scalar c = to.norm2() - radius2;

  return qf(a, b, c);
}

Vec3 Sphere::normal(Vec3 point) const
{
  return (point - position).normal();
}
