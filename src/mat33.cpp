#include "vec3.h"
#include "mat33.h"

using std::copy;
using std::swap;

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

scalar Mat33::det() const
{
  return v[0] * (v[4]*v[8] - v[5]*v[7])
    + v[1] * (v[5]*v[6] - v[3]*v[8])
    + v[2] * (v[3]*v[7] - v[4]*v[6]);
}

Mat33 operator*(scalar f, const Mat33& mat)
{
  return mat * f;
}

Mat33 Mat33::operator*(const Mat33& rhs) const
{
  return Mat33(
    v[0] * rhs.row[0] + v[1] * rhs.row[1] + v[2] * rhs.row[2],
    v[3] * rhs.row[0] + v[4] * rhs.row[1] + v[5] * rhs.row[2],
    v[6] * rhs.row[0] + v[7] * rhs.row[1] + v[8] * rhs.row[2]);
}

Vec3 operator*(const Vec3& v, const Mat33& mat)
{
  return mat.tmul(v);
}

Vec3 Mat33::operator *(const Vec3& v) const
{
  return Vec3{row[0].dot(v), row[1].dot(v), row[2].dot(v)};
}

Vec3 Mat33::tmul(const Vec3& v) const
{
  return row[0] * v[0] + row[1] * v[1] + row[2] * v[2];
}

scalar Mat33::trace() const
{
  return v[0] + v[4] + v[8];
}

Mat33 Mat33::inverse() const
{
  auto tr = trace();
  auto d = det();
  auto a2 = this->operator*(*this);
  return (identity * (0.5*(tr*tr - a2.trace())) - (*this)*tr + a2) / d;
}

Mat33 Mat33::from_diagonal(const Vec3& v)
{
  return Mat33({v.x, 0.0, 0.0, 0.0, v.y, 0.0, 0.0, 0.0, v.z});
}

Vec3 Mat33::diagonal() const
{
  return Vec3{v[0], v[4], v[8]};
}
