#include "spectrum.h"

const spectrum spectrum::zero{0.0};
const spectrum spectrum::one{1.0};

spectrum spectrum::clamp(scalar m, scalar M) const
{
  return spectrum{::max(m, ::min(x, M)), ::max(m, ::min(y, M)), ::max(m, ::min(z, M))};
}

spectrum spectrum::from_hex(uint hex)
{
  int b = hex % 0xff;
  int g = (hex >> 8) & 0xff;
  int r = (hex >> 16) & 0xff;
  const scalar denom = 256.0;
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
  return 0.3 * x + 0.54 * y + 0.16 * z;
}

ostream& operator<<(ostream& out, spectrum s)
{
  out << "{" << s.x << ", " << s.y << ", " << s.z << "}";
  return out;
}

