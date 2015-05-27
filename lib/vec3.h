#pragma once

#include "vector.h"

class Vec3 : public VectorT3<Vec3>
{
public: 
  Vec3() : VectorT3<Vec3>() { }
  explicit Vec3(scalar s) : VectorT3<Vec3>(s) { }
  Vec3(scalar x, scalar y, scalar z) : VectorT3<Vec3>(x, y, z) { }
  Vec3(VectorT3<Vec3>& s) : VectorT3<Vec3>(s) { }
  
  Vec3 normal() const
  {
    scalar len = norm();
    return Vec3(x / len, y / len, z / len);
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
  
  Vec3 projectOnto(Vec3 res) const
  {
    return res * (this->dot(res) / (res.dot(res)));
  }
  
  Vec3 rotateMatch(const Vec3& from, const Vec3& to) const;

  Vec3 rotateAxisAngle(const Vec3& axis, scalar angle) const;

  static Vec3 x_axis;
  static Vec3 y_axis;
  static Vec3 z_axis;
  static Vec3 zero;

private:
  Vec3 _rotateAxisAngle(const Vec3& axis, scalar cos_angle, scalar sin_angle) const;
};
