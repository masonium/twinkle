#include "texture.h"
#include "intersection.h"

spectrum Texture2D::at_point(const IntersectionView& isect) const
{
  return at_coord(isect.tc);
}

spectrum SolidColor::at_point(const IntersectionView& UNUSED(isect)) const
{
  return color;
}

spectrum SolidColor::at_coord(const Vec2& UNUSED(uv)) const
{
  return color;
}

Checkerboard2D::Checkerboard2D(spectrum a, spectrum b, int gsx, int gsy)
  : c1(a), c2(b), grid_size_x(gsx), grid_size_y(gsy > 0 ? gsy : gsx)
{
}

spectrum Checkerboard2D::at_coord(const Vec2& uv) const
{
  int m = std::floor(uv.u * grid_size_x) + std::floor(uv.v * grid_size_y);
  return m % 2 == 0 ? c1 : c2;
}

spectrum GridTexture2D::at_coord(const Vec2& uv) const
{
  scalar xf = fmod(fabs(uv.u) * grid_size, 1.0);
  scalar yf = fmod(fabs(uv.v) * grid_size, 1.0);

  if (min(xf, yf) < border_pct * 0.5 || max(xf, yf) > (1 - border_pct * 0.5))
    return border;
  else
    return solid;
}

spectrum NormalTexture::at_point(const IntersectionView& isect) const
{
  const auto& normal = 0.5 * (isect.normal + Vec3(1.0));
  return spectrum{normal.x, normal.y, normal.z};
}
