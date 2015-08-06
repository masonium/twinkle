#include "texture.h"

spectrum SolidColor::at_point(const Intersection& isect) const
{
  return color;
}

spectrum SolidColor::at_coord(const Vec2& uv) const
{
  return color;
}

Checkerboard2D::Checkerboard2D(spectrum a, spectrum b, int gsx, int gsy)
  : c1(a), c2(b), grid_size_x(gsx), grid_size_y(gsy > 0 ? gsy : gsx)
{
}

spectrum Checkerboard2D::at_coord(const Vec2& uv) const
{
  int m = int(uv.u * grid_size_x) + int(uv.v * grid_size_y);
  return m % 2 == 0 ? c1 : c2;
}

spectrum GridTexture2D::at_coord(const Vec2& uv) const
{
  scalar xf = fmod(uv.u * grid_size, 1.0);
  scalar yf = fmod(uv.v * grid_size, 1.0);

  if (min(xf, yf) < border_pct * 0.5 || max(xf, yf) > (1 - border_pct * 0.5))
    return border;
  else
    return solid;
}
