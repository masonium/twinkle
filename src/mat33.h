#pragma once

#include "vec3.h"

class Mat33
{
public:
  Mat33() {};

  template <typename Iter>
  Mat33(Iter begin, Iter end) {
    std::copy(begin, end, v);
  }

  Mat33(std::initializer_list<scalar> list);
  Mat33(const Vec3& r1, const Vec3& r2, const Vec3& r3);
  Mat33(const Mat33&);
  Mat33(Mat33&&);
  Mat33& operator=(const Mat33&);

  Mat33& operator*= (scalar f)
  {
    for (auto& x: v)
      x *= f;
    return *this;
  }
  Mat33& operator/= (scalar f);

  Mat33& operator +=(const Mat33& rhs);

  Mat33& operator -=(const Mat33& rhs);
  Mat33 operator -(const Mat33& rhs) const;

  Vec3 operator *(const Vec3& v) const;
  Vec3 tmul(const Vec3& v) const;

  Mat33 operator *(const Mat33& m) const;

  Mat33 operator*(scalar f) const
  {
    Mat33 x(*this);
    return (x *= f);
  }

  Mat33 operator /(scalar f) const;

  Mat33 transpose() const;
  Mat33 inverse() const;

  scalar det() const;
  scalar trace() const;

  Mat33 operator+(const Mat33& mat) const;

  /*
   * input axis must be normalized.
   */
  static Mat33 from_axis_angle(const Vec3& axis, scalar angle);

  /*
   * input axes must be normalized
   */
  static Mat33 rotate_match(const Vec3& axis_from, const Vec3& axis_to);
  static Mat33 rotate_to_z(const Vec3& axis_from);

  /*
   * Construct a matrix such that left-multiplying a vector by that matrix is
   * equivalent to taking the cross product of the axis and that vector.
   */
  static Mat33 cross_product_matrix(const Vec3& axis);

  static Mat33 identity;

  union
  {
    scalar v[9];
    Vec3 row[3];
  };

  static Mat33 z_to_y;
  static Mat33 y_to_z;

private:
  static Mat33 from_axis_angle(const Vec3& axis, scalar sa, scalar ca);
};

Vec3 operator*(const Vec3& v, const Mat33& mat);
