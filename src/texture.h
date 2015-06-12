#pragma once

#include "spectrum.h"
#include "vec3.h"
#include "intersection.h"

class Texture
{
public:
  virtual spectrum at_point(const Intersection& isect) const = 0;
  virtual ~Texture() { }
};

class SolidColor : public Texture
{
public:
  SolidColor(spectrum x = spectrum::zero) : color(x) {}

  spectrum at_point(const Intersection& isect) const override
  {
    return color;
  }
  
private:
  spectrum color;
};

class GridTexture2D : public Texture
{
public:
  GridTexture2D(spectrum solid_, spectrum border_,
                scalar grid_size_, scalar border_pct_) 
    : solid(solid_), border(border_), grid_size(grid_size_),
      border_pct(border_pct_) {}

  spectrum at_point(const Intersection& isect) const override;
  
private:
  const spectrum solid, border;
  const scalar grid_size, border_pct;
};
