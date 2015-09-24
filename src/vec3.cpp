#include "vec3.h"
#include "mat33.h"
#include "math_util.h"
#include <vector>

using std::copy;
using std::make_pair;
using std::swap;

Vec3 Vec3::x_axis{1.0, 0.0, 0.0};
Vec3 Vec3::y_axis{0.0, 1.0, 0.0};
Vec3 Vec3::z_axis{0.0, 0.0, 1.0};
Vec3 Vec3::zero{0.0, 0.0, 0.0};

Mat33 Mat33::z_to_y = Mat33::rotate_match(Vec3::z_axis, Vec3::y_axis);
Mat33 Mat33::y_to_z = Mat33::rotate_match(Vec3::y_axis, Vec3::z_axis);

Vec3::Vec3(const Vec3& rhs) : VectorT3<Vec3>{rhs}
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
}

Vec3& Vec3::operator=(const Vec3& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  return *this;
}
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
  Vec3 invariant = this->project_onto(axis);
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

pair<scalar, scalar> Vec3::to_euler() const
{
  scalar phi = acos(z);
  scalar theta = atan2(y, x);
  if (theta < 0)
    theta += 2 * PI;

  return make_pair(theta, phi);
}

Mat33 Vec3::tensor_product(const Vec3& a) const
{
  return Mat33{*this * a.x, *this * a.y, *this * a.z};
}

////////////////////////////////////////////////////////////////////////////////


Vec3 min(const Vec3& a, const Vec3& b)
{
  return Vec3{std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
}
Vec3 max(const Vec3& a, const Vec3& b)
{
  return Vec3{std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}
ostream& operator <<(ostream& out, const Vec3& v)
{
  out << v.x << ' ' << v.y << ' ' << v.z;
  return out;
}
