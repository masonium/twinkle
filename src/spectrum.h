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
class spectrum_XYZ;
class spectrum_xyY;

/*
 * spectrum represents colors in linear rgb format. It should probably be called
 * spectrum_rgb to reflect this fact.
 */
class spectrum : public VectorT3<spectrum>
{
public:
  spectrum() : VectorT3<spectrum>() { }
  explicit spectrum(scalar s) : VectorT3<spectrum>(s) { }
  explicit spectrum(scalar x, scalar y, scalar z) : VectorT3<spectrum>(x, y, z) { }
  spectrum(const spectrum& s) : VectorT3<spectrum>(s.x, s.y, s.z) { }

  spectrum(const spectrum_XYZ&);
  
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

  operator spectrum_XYZ() const;

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
  spectrum_xyY(const spectrum_XYZ&);

  scalar luminance() const { return Y; }

  operator spectrum_XYZ() const;

  union
  {
    struct
    {
      scalar x, y, Y;
    };
    scalar v[3];
  };
};

class spectrum_XYZ : public VectorT3<spectrum_XYZ>
{
public:

  spectrum_XYZ() : VectorT3<spectrum_XYZ>() { }
  explicit spectrum_XYZ(scalar x, scalar y, scalar z) : VectorT3<spectrum_XYZ>(x, y, z) { }
  spectrum_XYZ(const spectrum_XYZ& s) : VectorT3<spectrum_XYZ>(s.x, s.y, s.z) { }

  using VectorT3<spectrum_XYZ>::operator=;

  scalar luminance() const { return y; }

  spectrum_XYZ& operator/=(const spectrum_XYZ& r)
  {
    this->x /= r.x;
    this->y /= r.y;
    this->z /= r.z;
    return *this;
  }
  spectrum_XYZ operator/(const spectrum_XYZ& r) const
  {
    spectrum_XYZ a = *this;
    return (a /= r); 
  }

};

scalar tvi(scalar luminance);

ostream&  operator<<(ostream& out, spectrum s);

