#include "vec3.h"
#include "math_util.h"

Vec3 Vec3::x_axis{1.0, 0.0, 0.0};
Vec3 Vec3::y_axis{0.0, 1.0, 0.0};
Vec3 Vec3::z_axis{0.0, 0.0, 1.0};
Vec3 Vec3::zero{0.0, 0.0, 0.0};

/**
 * Compute a vector from euler angles. Assumes that (0, 0, 1) is the north pole
 * (i.e. at phi = 0). The resultant vector is normalized.
 */
Vec3 Vec3::from_euler(scalar theta, scalar phi)
{
  scalar s = sin(phi);
  return Vec3{s * scalar(cos(theta)), s * scalar(sin(theta)), scalar(cos(phi))};
}

/**
 * Given a model of a rotation from 'from' to 'to', rotate self in the same
 * manner.
 *
 * Assumes from and to are both normal.
 */
Vec3 Vec3::rotateMatch(const Vec3& from, const Vec3& to) const
{
  auto dot = from.dot(to);
  if (dot > 0.9999)
    return *this;
  if (dot < -0.9999)
    return -(*this);

  auto c = from.cross(to);
  scalar s = c.norm();

  return _rotateAxisAngle(c / s, dot, s);
}

Vec3 Vec3::rotateAxisAngle(const Vec3& axis, scalar angle) const
{
  return _rotateAxisAngle(axis, cos(angle), sin(angle));
}

Vec3 Vec3::_rotateAxisAngle(const Vec3& axis, scalar c, scalar s) const
{
  Vec3 invariant = this->projectOnto(axis);
  Vec3 x = *this - invariant;
  Vec3 y = axis.cross(x);

  return invariant + (x * c) + (y * s);
}

void Vec3::to_euler(scalar& theta, scalar& phi) const
{
  phi = acos(z);
  theta = atan2(y, x);
  if (theta < 0)
    theta += 2 * PI;
}
