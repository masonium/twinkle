#include <iostream>
#include "spectrum.h"
#include "util.h"

using std::cout;
using std::cerr;

const spectrum spectrum::zero{0.0};
const spectrum spectrum::one{1.0};

const scalar CIE_RGB_TO_XYZ[3][3] = {{0.4887180,  0.3106803,  0.2006017},
                                     {0.1762044,  0.8129847,  0.0108109},
                                     {0.0000000,  0.0102048,  0.9897952}};

const scalar CIE_XYZ_TO_RGB[3][3] = {{2.3706743, -0.9000405 -0.4706338},
                                     {-0.5138850, 1.4253036, 0.0885814},
                                     {0.0052982, -0.0146949, 1.0093968}};

spectrum::spectrum(const spectrum_XYZ& xyz)
{
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      v[i] += CIE_XYZ_TO_RGB[i][j] * xyz[j];
}

spectrum::operator spectrum_XYZ() const
{
  spectrum_XYZ xyz;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      xyz.v[i] += CIE_RGB_TO_XYZ[i][j] * v[j];

  return xyz;
}


spectrum spectrum::clamp(scalar m, scalar M) const
{
  return spectrum{::max(m, ::min(x, M)), ::max(m, ::min(y, M)), ::max(m, ::min(z, M))};
}

spectrum spectrum::from_hex(uint hex)
{
  uint b = hex & 0xff;
  uint g = (hex >> 8) & 0xff;
  uint r = (hex >> 16) & 0xff;
  const scalar denom = 255.0;
  return spectrum{r / denom, g / denom, b / denom};
}

spectrum spectrum::from_hsv(scalar hue, scalar sat, scalar value)
{
  const scalar c = sat * value;
  double intval;
  
  const scalar x = c * (1 - fabs( modf(hue / 120.0, &intval) * 2 - 1));

  const scalar m = value - c;
  scalar r = 0, g = 0, b = 0;
  if (hue < 60)
  {
    r = c; g = x;
  }
  else if (hue < 120)
  {
    r = x; g = c;
  }
  else if (hue < 180)
  {
    g = c; b = x;
  }
  else if (hue < 240)
  {
    g = x; b = c;
  }
  else if (hue < 300)
  {
    r = x; b = c;
  }
  else
  {
    r = c; b = x;
  }

  return spectrum{(r + m), (g + m), (b + m)};
}

spectrum spectrum::max(const spectrum &a, const spectrum& b)
{
  return spectrum{::max(a.x, b.x), ::max(a.y, b.y), ::max(a.z, b.z)};
}

scalar spectrum::luminance() const
{
  return CIE_RGB_TO_XYZ[1][0] * x + CIE_RGB_TO_XYZ[1][1] * y + CIE_RGB_TO_XYZ[1][2] * z;
}

scalar tvi(scalar luminance)
{
  luminance = std::max<scalar>(luminance, 0.00001);

  scalar logl = log10(luminance);
  scalar log_tvi;
  if (logl <= -3.94)
    log_tvi = -2.86;
  else if (logl >= -1.44)
    log_tvi = logl - 0.395;
  else
    log_tvi = pow(0.405 * logl + 1.6, 2.18) - 2.86;

  scalar t = pow(10, log_tvi);
  return t;
}

spectrum spectrum::rescale_luminance(scalar nl) const
{
  scalar L = luminance();
  if (L == 0)
    return spectrum::zero;

  scalar m = ::max(x, ::max(y, z));
  spectrum coef = *this / m;

  const scalar max_denom = coef.luminance();

  return coef * nl / max_denom;
}

ostream& operator<<(ostream& out, spectrum s)
{
  out << "{" << s.x << ", " << s.y << ", " << s.z << "}";
  return out;
}

////////////////////////////////////////////////////////////////////////////////

spectrum_xyY::spectrum_xyY(const spectrum_XYZ& xyz)
{
  auto denom = xyz.x + xyz.y + xyz.z;
  x = xyz.x / denom;
  Y = xyz.y;
  y = xyz.z / denom;
}

spectrum_xyY::operator spectrum_XYZ() const
{
  return spectrum_XYZ{x * Y / y, Y, (1 - x - y) * Y / y};
}

////////////////////////////////////////////////////////////////////////////////
