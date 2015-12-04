#pragma once

#include "spectrum.h"
#include "vec2.h"
#include "vec3.h"

class IntersectionView;

class Texture
{
public:
  virtual spectrum at_point(const IntersectionView& isect) const = 0;
  virtual ~Texture() { }
};

class Texture2D : public Texture
{
public:
  virtual spectrum at_point(const IntersectionView& isect) const override;
  
  virtual spectrum at_coord(const Vec2& uv) const = 0;
};

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
    : solid(solid_), border(border_), check_size(check_size_),
      border_pct(border_pct_) {}

  spectrum at_coord(const Vec2& uv) const override;
  
private:
  const spectrum solid, border;
  const scalar check_size, border_pct;
};

class NormalTexture : public Texture
{
public:
  spectrum at_point(const IntersectionView& isect) const;
};
