#include "sphere.h"

using std::move;

scalar Sphere::intersect(const Ray& r) const
{
  const Vec3 to = r.position - position;

  const scalar a = r.direction.norm2();
  const scalar b = 2.0 * r.direction.dot(to);
  const scalar c = to.norm2() - radius2;

  scalar res = qf(a, b, c);

  if (res > -0.0001)
    return max<scalar>(res, 0.0001);
  else
    return res;
}

Vec3 Sphere::normal(Vec3 point) const
{
  return (point - position).normal();
}

Vec3 Sphere::sample_shadow_ray_dir(const Intersection& isect,
                             scalar r1, scalar r2) const
{
  scalar theta = 2 * PI * r1;
  scalar phi = 2 * asin(sqrt(r2));
  
  return move(Vec3::from_euler(theta, phi) * radius + position - isect.position);
}
