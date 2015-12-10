#pragma once

#include "vector.h"
#include <memory>
#include <cmath>

using std::pair;

class Mat33;

class Vec3 : public VectorT3<Vec3>
{
public: 
  Vec3() : VectorT3<Vec3>() { }
  explicit Vec3(scalar s) : VectorT3<Vec3>(s) { }
  Vec3(scalar x, scalar y, scalar z) : VectorT3<Vec3>(x, y, z) { }
  Vec3(VectorT3<Vec3>& s) : VectorT3<Vec3>(s) { }

  Vec3& operator=(const Vec3& x);
  Vec3(const Vec3&);
  Vec3(Vec3&&) = default;
  
  Vec3 normal() const
  {
    scalar len = norm();
    return Vec3(x / len, y / len, z / len);
  }

  using VectorT3<Vec3>::operator+;
  using VectorT3<Vec3>::operator/;

  scalar min() const { return std::min(x, std::min(y, z)); }
  scalar max() const { return std::max(x, std::max(y, z)); }

  /**
   * Return the index of the minumum element, with a preference for the lower
   * indexed elements.
   */
  int min_element() const;

  /**
   * Return the index of the maximum element, with a preference for the lower
   * indexed elements.
   */
  int max_element() const;


  Vec3 elem_div(const Vec3& rhs) const
  {
    return Vec3{x / rhs.x, y / rhs.y, z / rhs.z};
  }
  Vec3 elem_mult(const Vec3& rhs) const
  {
    return Vec3{x * rhs.x, y * rhs.y, z * rhs.z};
  }
  Vec3 abs() const
  {
    return Vec3(fabs(x), fabs(y), fabs(z));
  }

  Vec3 cross(const Vec3& other) const
  {
    return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
  }

  /*
   * Return the vector corresponding to given euler angles. Assumes phi == 0 is
   * the (0, 0, 1) vector.
   */
  static Vec3 from_euler(scalar theta, scalar phi);

  /*
   * Assumes that the calling vector is normal.
   */
  void to_euler(scalar& theta, scalar& phi) const;
  pair<scalar, scalar> to_euler() const;
  
  Vec3 project_onto(const Vec3& res) const
  {
    return res * (this->dot(res) / (res.dot(res)));
  }

  Vec3 reflect_over(const Vec3& n) const
  {
    Vec3 proj = project_onto(n);
    return 2.0 * proj - *this;
  }
  
  Vec3 rotateMatch(const Vec3& from, const Vec3& to) const;

  Vec3 rotateAxisAngle(const Vec3& axis, scalar angle) const;

  Mat33 tensor_product(const Vec3& a) const;
  
  static Vec3 x_axis;
  static Vec3 y_axis;
  static Vec3 z_axis;
  static Vec3 zero;

private:
  Vec3 _rotateAxisAngle(const Vec3& axis, scalar cos_angle, scalar sin_angle) const;
};


ostream& operator <<(ostream& out, const Vec3& v);

Vec3 min(const Vec3& a, const Vec3& b);
Vec3 max(const Vec3& a, const Vec3& b);
