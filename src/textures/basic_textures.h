#pragma once

#include "texture.h"

class SolidColor : public Texture2D
{
public:
  SolidColor(spectrum x = spectrum::zero) : color(x) {}

  spectrum at_point(const IntersectionView& isect) const override;
  spectrum at_coord(const Vec2& uv) const override;

private:
  spectrum color;
};

class Checkerboard2D : public Texture2D
{
public:
  Checkerboard2D(spectrum a, spectrum b, scalar cs, scalar csy = 0.0);

  spectrum at_coord(const Vec2& uv) const override;

private:
  spectrum c1, c2;
  scalar check_size_x, check_size_y;
};

class Gradient2D : public Texture2D
{
public:
  Gradient2D() { }

  spectrum at_coord(const Vec2& uv) const override;
};

class GridTexture2D : public Texture2D
{
public:
  GridTexture2D(spectrum solid_, spectrum border_,
                scalar check_size_, scalar border_pct_)
    : solid(solid_), border(border_), csx(check_size_),
      csy(check_size_),
      border_pct(border_pct_) {}

  GridTexture2D(spectrum solid_, spectrum border_,
                scalar check_size_x_, scalar check_size_y_,
                scalar border_pct_)
    : solid(solid_), border(border_), csx(check_size_x_),
      csy(check_size_y_), border_pct(border_pct_) {}

  spectrum at_coord(const Vec2& uv) const override;

private:
  const spectrum solid, border;
  const scalar csx, csy, border_pct;
};

class NormalTexture : public Texture
{
public:
  spectrum at_point(const IntersectionView& isect) const;
};
