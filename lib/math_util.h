#pragma once

#include "twinkle.h"
#include <cmath>
#include <memory>

using std::move;
const scalar PI = 4.0 * atan(1.0);

template <typename vec_type>
class VectorT3
{
public:
  VectorT3(scalar x_, scalar y_, scalar z_) : x(x_), y(y_), z(z_)
  {
  }

  explicit VectorT3(scalar f) : x(f), y(f), z(f)
  {
  }
  
  VectorT3() : VectorT3(0.0, 0.0, 0.0)
  {
  }
  VectorT3(const VectorT3<vec_type>& a) = default;

  VectorT3(VectorT3<vec_type>& a) : x(a.x), y(a.y), z(a.z)
  {
  }
  
  union 
  {
    struct
    {
      scalar x, y, z;
    };
    struct
    {
      scalar v[3];
    };
  };

  vec_type& operator+= (const vec_type& r)
  {
    this->x += r.x;
    this->y += r.y;
    this->z += r.z;
    return static_cast<vec_type&>(*this);
  }
  
  vec_type operator+(const vec_type& r) const
  {
    auto a = *this;
    return static_cast<vec_type>(a += r); 
  }

  vec_type& operator-= (const vec_type& r)
  {
    this->x -= r.x;
    this->y -= r.y;
    this->z -= r.z;
    return static_cast<vec_type&>(*this);
  }
  
  vec_type operator-(const vec_type& r) const
  {
    auto a = *this;
    return static_cast<vec_type>(a -= r); 
  }

  vec_type operator-() const
  {
    return vec_type(-x, -y, -z);
  }
  
  vec_type& operator*=(scalar a)
  {
    this->x *= a;
    this->y *= a;
    this->z *= a;
    return static_cast<vec_type&>(*this);
  }
  
  vec_type operator*(scalar a) const
  {
    auto x = *this;
    return static_cast<vec_type>(x *= a); 
  }
  
  vec_type& operator/= (scalar a)
  {
    this->x /= a;
    this->y /= a;
    this->z /= a;
    return static_cast<vec_type&>(*this);
  }
  
  vec_type operator/(scalar a) const
  {
    auto x = *this;
    return static_cast<vec_type>(x /= a); 
  }

  scalar dot(const vec_type& other) const
  {
    return x * other.x + y*other.y + z * other.z;
  }
  
  scalar norm2() const
  {
    return dot(static_cast<const vec_type&>(*this));
  }
  scalar norm() const
  {
    return sqrt(norm2());
  }
  
};

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

class Ray
{
public:
  Ray(Vec3 pos, Vec3 dir) : position(pos), direction(dir)
  {

  }

  Vec3 evaluate(scalar t) const
  {
    return position + direction * t;
  }
  
  Ray& normalize() 
  {
    direction = direction.normal();
    return *this;
  }

  Ray nudge(scalar eps = SURFACE_EPSILON) const;
  
  Vec3 position, direction;
};


scalar qf(scalar a, scalar b, scalar c);

template<typename T>
scalar norm(const T& s)
{
  return s.norm();
}

scalar norm(const scalar& s);
