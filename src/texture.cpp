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

Checkerboard2D::Checkerboard2D(spectrum a, spectrum b, scalar csx, scalar csy)
  : c1(a), c2(b), check_size_x(csx), check_size_y(csy > 0 ? csy : csx)
{
}

spectrum Checkerboard2D::at_coord(const Vec2& uv) const
{
  int m = std::floor(uv.u / check_size_x) + std::floor(uv.v / check_size_y);
  return m % 2 == 0 ? c1 : c2;
}

spectrum GridTexture2D::at_coord(const Vec2& uv) const
{
  scalar xf = fmod(fabs(uv.u) / check_size, 1.0);
  scalar yf = fmod(fabs(uv.v) / check_size, 1.0);

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

spectrum Gradient2D::at_coord(const Vec2& uv) const
{
  return spectrum{uv.u, 0, uv.v};
}
