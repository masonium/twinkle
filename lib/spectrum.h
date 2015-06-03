#pragma once

#include "math_util.h"
#include "vector.h"
#include <iostream>
#include <string>

using std::min;
using std::max;
using std::move;
using std::string;
using std::ostream;

class spectrum : public VectorT3<spectrum>
{
public:
  spectrum() : VectorT3<spectrum>() { }
  explicit spectrum(scalar s) : VectorT3<spectrum>(s) { }
  explicit spectrum(scalar x, scalar y, scalar z) : VectorT3<spectrum>(x, y, z) { }
  spectrum(const spectrum& s) : VectorT3<spectrum>(s.x, s.y, s.z) { }
  
  spectrum& operator=(const spectrum& rhs) 
  {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
  }
  
  spectrum& operator*=(const spectrum& r)
  {
    this->x *= r.x;
    this->y *= r.y;
    this->z *= r.z;
    return *this;
  }

  using VectorT3<spectrum>::operator*;
  using VectorT3<spectrum>::operator*=;

  using VectorT3<spectrum>::operator/;
  using VectorT3<spectrum>::operator/=;

  spectrum operator*(const spectrum& r) const
  {
    spectrum a = *this;
    return (a *= r); 
  }

  spectrum& operator/=(const spectrum& r)
  {
    this->x /= r.x;
    this->y /= r.y;
    this->z /= r.z;
    return *this;
  }
  spectrum operator/(const spectrum& r) const
  {
    spectrum a = *this;
    return (a /= r); 
  }
  
  spectrum clamp(scalar m, scalar M) const;

  static spectrum max(const spectrum &a, const spectrum& b);
  static spectrum from_hex(uint hex_color);
  static spectrum from_hsv(scalar hue, scalar sat, scalar value);

  scalar luminance() const;
  
  static const spectrum one;
  static const spectrum zero;
};

ostream&  operator<<(ostream& out, spectrum s);
