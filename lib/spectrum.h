#pragma once

#include "math_util.h"
#include <iostream>

using std::min;
using std::max;
using std::move;
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
  spectrum operator*(const spectrum& r) const
  {
    spectrum a = *this;
    return (a *= r); 
  }
  spectrum clamp(scalar m, scalar M) const
  {
    return spectrum{max(m, min(x, M)), max(m, min(y, M)), max(m, min(z, M))};
  }

  static const spectrum one;
  static const spectrum zero;
};

ostream&  operator<<(ostream& out, spectrum s);
