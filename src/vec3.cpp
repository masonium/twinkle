#include "vec3.h"
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

Vec3::Vec3(const Vec3& rhs)
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

////////////////////////////////////////////////////////////////////////////////

Mat33& Mat33::operator=(const Mat33& mat)
{
  copy(mat.v, mat.v+9, v);
  return *this;
}
Mat33 Mat33::identity{1, 0, 0, 0, 1, 0, 0, 0, 1};

Mat33::Mat33(std::initializer_list<scalar> lst)
{
  copy(lst.begin(), lst.end(), v);
}

Mat33::Mat33(const Mat33& x)
{
  copy(x.v, x.v+9, v);
}

Mat33::Mat33(Mat33&& x)
{
  swap(v, x.v);
}

Mat33::Mat33(const Vec3& r1, const Vec3& r2, const Vec3&r3) :
  row{r1, r2, r3}
{

}

Mat33& Mat33::operator +=(const Mat33& rhs)
{
  for (int i = 0; i < 9; ++i)
    v[i] += rhs.v[i];
  return *this;
}

Mat33 Mat33::operator+(const Mat33& v) const
{
  Mat33 x(*this);
  return x += v;
}

Mat33& Mat33::operator -=(const Mat33& rhs)
{
  for (int i = 0; i < 9; ++i)
    v[i] -= rhs.v[i];
  return *this;
}

Mat33 Mat33::operator-(const Mat33& v) const
{
  Mat33 x(*this);
  return x -= v;
}

Mat33& Mat33::operator/=(scalar f)
{
  for (auto& x: v)
    x /= f;
  return *this;
}

Mat33 Mat33::operator/(scalar f) const
{
  Mat33 x(*this);
  return x /= f;
}


Mat33 Mat33::transpose() const
{
  return Mat33{v[0], v[3], v[6], v[1], v[4], v[7], v[2], v[5], v[8]};
}

Mat33 Mat33::rotate_match(const Vec3& axis_from, const Vec3& axis_to)
{
  const scalar c = axis_from.dot(axis_to);

  if (c > 1 - EPSILON)
    return Mat33::identity;

  const auto raxis = axis_from.cross(axis_to);
  const scalar s = raxis.norm();

  return from_axis_angle(raxis / s, s, c);
}

Mat33 Mat33::rotate_to_z(const Vec3& axis_from)
{
  const scalar c = axis_from.z;

  const scalar s = sqrt(axis_from.x * axis_from.x + axis_from.y * axis_from.y);
  if (s < EPSILON)
    return Mat33::identity;

  const auto raxis = Vec3{axis_from.y, -axis_from.x, 0};

  return from_axis_angle(raxis / s, s, c);
}

Mat33 Mat33::from_axis_angle(const Vec3& axis, scalar angle)
{
  const scalar s = sin(angle), c = cos(angle);
  return from_axis_angle(axis, s, c);
}

Mat33 Mat33::from_axis_angle(const Vec3& axis, scalar sa, scalar ca)
{
  return ca * Mat33::identity + sa * cross_product_matrix(axis)
    + (1 - ca) * axis.tensor_product(axis);
}

Mat33 Mat33::cross_product_matrix(const Vec3& a)
{
  return Mat33{0, -a.z, a.y, a.z, 0, -a.x, -a.y, a.x, 0};
}
