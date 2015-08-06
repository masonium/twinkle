#pragma once

#include "spectrum.h"
#include "vec2.h"
#include "vec3.h"
#include "intersection.h"

class Texture
{
public:
  virtual spectrum at_point(const Intersection& isect) const = 0;
  virtual ~Texture() { }
};

class Texture2D : public Texture
{
public:
  virtual spectrum at_point(const Intersection& isect) const override
  {
    return at_coord(isect.tc);
  }
  
  virtual spectrum at_coord(const Vec2& uv) const = 0;
};

class SolidColor : public Texture2D
{
public:
  SolidColor(spectrum x = spectrum::zero) : color(x) {}

  spectrum at_point(const Intersection& isect) const override;
  spectrum at_coord(const Vec2& uv) const override;

private:
  spectrum color;
};

class Checkerboard2D : public Texture2D
{
public:
  Checkerboard2D(spectrum a, spectrum b, int gs, int gsy = 0);

  spectrum at_coord(const Vec2& uv) const override;

private:
  spectrum c1, c2;
  int grid_size_x, grid_size_y;
};

class Gradient2D : public Texture2D
{
public:
  Gradient2D() { }

  spectrum at_coord(const Vec2& uv) const override
  {
    return spectrum{uv.u, 0, uv.v};
  }
};

class GridTexture2D : public Texture2D
{
public:
  GridTexture2D(spectrum solid_, spectrum border_,
                scalar grid_size_, scalar border_pct_) 
    : solid(solid_), border(border_), grid_size(grid_size_),
      border_pct(border_pct_) {}

  spectrum at_coord(const Vec2& uv) const override;
  
private:
  const spectrum solid, border;
  const scalar grid_size, border_pct;
};
