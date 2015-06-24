#include "vec3.h"
#include "mat33.h"

scalar Mat33::det() const
{
  return v[0] * (v[4]*v[8] - v[5]*v[7])
    + v[1] * (v[5]*v[6] - v[3]*v[8])
    + v[2] * (v[3]*v[7] - v[4]*v[6]);
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
