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

class spectrum;
class specturm_xyz;
class specturm_xyY;

/**
 * spectrum_xyY is a class representing colors following the CIE standard xyY
 * color space. It is used in practice in most CIE standard specifications, as
 * opposed to the XYZ space, whose benefits are more abstract.
 */
class spectrum_xyY
{
public:
  spectrum_xyY() : x(0), y(0), Y(0) { }
  spectrum_xyY(scalar a, scalar b, scalar c): x(a), y(b), Y(c)
  {
  };

  union
  {
    struct
    {
      scalar x, y, Y;
    };
    scalar v[3];
  };
};

class spectrum : public VectorT3<spectrum>
{
public:
  spectrum() : VectorT3<spectrum>() { }
  explicit spectrum(scalar s) : VectorT3<spectrum>(s) { }
  explicit spectrum(scalar x, scalar y, scalar z) : VectorT3<spectrum>(x, y, z) { }
  spectrum(const spectrum& s) : VectorT3<spectrum>(s.x, s.y, s.z) { }
  
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
  
  spectrum clamp(scalar m = 0.0, scalar M = 1.0) const;

  static spectrum max(const spectrum &a, const spectrum& b);
  static spectrum from_hex(uint hex_color);
  static spectrum from_hsv(scalar hue, scalar sat, scalar value);

  scalar luminance() const;
  spectrum rescale_luminance(scalar new_luminance) const;
  
  static const spectrum one;
  static const spectrum zero;
};

class spectrum_xyz : public VectorT3<spectrum_xyz>
{
  spectrum_xyz() : VectorT3<spectrum_xyz>() { }
  explicit spectrum_xyz(scalar x, scalar y, scalar z) : VectorT3<spectrum_xyz>(x, y, z) { }
  spectrum_xyz(const spectrum_xyz& s) : VectorT3<spectrum_xyz>(s.x, s.y, s.z) { }

  using VectorT3<spectrum_xyz>::operator=;

  spectrum_xyz& operator/=(const spectrum_xyz& r)
  {
    this->x /= r.x;
    this->y /= r.y;
    this->z /= r.z;
    return *this;
  }
  spectrum_xyz operator/(const spectrum_xyz& r) const
  {
    spectrum_xyz a = *this;
    return (a /= r); 
  }

};

ostream&  operator<<(ostream& out, spectrum s);
