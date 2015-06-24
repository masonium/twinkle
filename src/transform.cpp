#include "transform.h"

Transform::Transform(Mat33 A_, Vec3 b_)
  : A(A_), Ainv(A.inverse()), b(b_)
{
}

Ray Transform::transform_ray(const Ray& r) const
{
  return Ray(transform_point(r.position), transform_direction(r.direction));
}

Vec3 Transform::transform_point(const Vec3& p) const
{
  return A * p + b;
}

Vec3 Transform::transform_direction(const Vec3& d) const
{
  return A * d;
}

Vec3 Transform::transform_normal(const Vec3& n) const
{
  return (n * Ainv).normal();
}



Ray Transform::inv_transform_ray(const Ray& r) const
{
  return Ray(inv_transform_point(r.position), inv_transform_direction(r.direction));
}

Vec3 Transform::inv_transform_point(const Vec3& p) const
{
  return Ainv * (p - b);
}

Vec3 Transform::inv_transform_direction(const Vec3& d) const
{
  return Ainv * d;
}

Vec3 Transform::inv_transform_normal(const Vec3& n) const
{
  return (n * A).normal();
}
