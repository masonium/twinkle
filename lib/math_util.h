#pragma once

#include "twinkle.h"
#include <cmath>
#include <memory>

using std::move;
const scalar PI = 4.0 * atan(1.0);

class Vec3
{
public:
  Vec3(scalar x_, scalar y_, scalar z_) : x(x_), y(y_), z(z_)
  {
  }

  Vec3() : Vec3(0.0, 0.0, 0.0)
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

  Vec3& operator+= (const Vec3& r)
  {
    this->x += r.x;
    this->y += r.y;
    this->z += r.z;
    return *this;
  }
  Vec3 operator+(const Vec3& r) const
  {
    Vec3 a = *this;
    return (a += r); 
  }

  Vec3& operator-= (const Vec3& r)
  {
    this->x -= r.x;
    this->y -= r.y;
    this->z -= r.z;
    return *this;
  }
  Vec3 operator-(const Vec3& r) const
  {
    Vec3 a = *this;
    return (a -= r); 
  }
  Vec3& operator*= (scalar a)
  {
    this->x *= a;
    this->y *= a;
    this->z *= a;
    return *this;
  }
  Vec3 operator*(scalar a) const
  {
    Vec3 x = *this;
    return (x *= a); 
  }
  Vec3& operator/= (scalar a)
  {
    this->x /= a;
    this->y /= a;
    this->z /= a;
    return *this;
  }
  Vec3 operator/(scalar a) const
  {
    Vec3 x = *this;
    return (x /= a); 
  }

  scalar dot(const Vec3& other) const
  {
    return x * other.x + y*other.y + z * other.z;
  }

  Vec3 cross(const Vec3& other) const
  {
    return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
  }
  scalar norm2() const
  {
    return dot(*this);
  }
  scalar norm() const
  {
    return sqrt(norm2());
  }
  
  Vec3&& normal() const
  {
    scalar len = norm();
    return std::move(Vec3(x / len, y / len, z / len));
  }
  
  Vec3 projectOnto(Vec3 res) const
  {
    return res * (this->dot(res) / (res.dot(res)));
  }
  
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

  
  Vec3 position, direction;
};

scalar qf(scalar a, scalar b, scalar c);
