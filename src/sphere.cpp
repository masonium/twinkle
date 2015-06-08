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

Vec3 Sphere::normal(const Vec3& point) const
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

void Sphere::texture_coord(const Vec3& pos, const Vec3& normal,
                           scalar& u, scalar& v, Vec3& dpdu, Vec3& dpdv) const
{
  scalar theta, phi;
  normal.to_euler(theta, phi);
  v = phi / PI;
  u = theta / (2 * PI);
  
  dpdu = Vec3(-sin(theta), cos(theta), 0);
  dpdv = normal.cross(dpdu);
}
