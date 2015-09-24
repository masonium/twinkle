#include "sphere.h"

using std::move;

scalar_fp Sphere::intersect(const Ray& r) const
{
  const Vec3 to = r.position - position;

  const scalar a = r.direction.norm2();
  const scalar b = 2.0 * r.direction.dot(to);
  const scalar c = to.norm2() - radius2;

  scalar res = qf(a, b, c);

  if (res > -0.0001)
    return scalar_fp{max<scalar>(res, 0.0001)};
  else
    return sfp_none;
}

Vec3 Sphere::normal(SubGeo geo, const Vec3& point) const
{
  return (point - position).normal();
}

/*
Vec3 Sphere::sample_shadow_ray_dir(const Intersection& isect,
                             scalar r1, scalar r2) const
{
  scalar theta = 2 * PI * r1;
  scalar phi = 2 * asin(sqrt(r2));
  
  return move(Vec3::from_euler(theta, phi) * radius + position - isect.position);
}
*/

void Sphere::texture_coord(SubGeo geo, const Vec3& pos, const Vec3& normal,
                           scalar& u, scalar& v) const
{
  scalar theta, phi;
  normal.to_euler(theta, phi);
  v = phi / PI;
  u = theta / (2 * PI);
}

bounds::AABB Sphere::get_bounding_box() const
{
  auto vr = Vec3(radius);
  return bounds::AABB(position - vr, position + vr);
}
