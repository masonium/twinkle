#pragma once

#include "twinkle.h"
#include <cmath>
#include <iostream>

using std::istream;
using std::ostream;

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
  vec_type& operator=(const vec_type& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
  };

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
  
  scalar operator[](int idx) const
  {
    return v[idx];
  }
  scalar& operator[](int idx)
  {
    return v[idx];
  }

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

  bool operator>=(scalar a) const
  {
    return x >= a && y >= a && z >= a;
  }

  bool operator>(scalar a) const
  {
    return x > a && y > a && z > a;
  }

  bool operator<=(scalar a) const
  {
    return x <= a && y <= a && z <= a;
  }

  bool operator<(scalar a) const
  {
    return x < a && y < a && z < a;
  }

  bool operator==(const vec_type& other) const
  {
    return (x == other.x) && (y == other.y) && (z == other.z);
  }

  bool is_bounded() const
  {
    return x != SCALAR_MAX && x != SCALAR_MIN && y != SCALAR_MAX &&
      y != SCALAR_MIN && z != SCALAR_MAX && z != SCALAR_MIN;
  }

  bool isnan() const
  {
    return std::isnan(x) || std::isnan(y) || std::isnan(z);
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

  static vec_type deserialize(istream& in);
  void serialize(ostream& out)
  {
    out.write(reinterpret_cast<char*>(v), sizeof(v));
  }
};

template <typename vec_type>
bool lexographic_compare(const VectorT3<vec_type>& a, const VectorT3<vec_type>& b)
{
  if (a.x < b.x) return true;
  if (a.x > b.x) return false;
  if (a.y < b.y) return true;
  if (a.y > b.y) return false;
  return a.z < b.z;
}

template <typename vec_type>
vec_type operator *(scalar f, const VectorT3<vec_type>& vec)
{
  return vec * f;
}

template <typename vec_type>
vec_type VectorT3<vec_type>::deserialize(istream& in)
{
  vec_type x;
  in.read(reinterpret_cast<char*>(x.v), sizeof(x.v));
  return x;
}

template <typename vec_type>
istream& operator >>(istream& in, VectorT3<vec_type>& v)
{
  in >> v.x >> v.y >> v.z;
  return in;
}
