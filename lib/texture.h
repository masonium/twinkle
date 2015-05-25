#pragma once

#include "spectrum.h"

class Texture
{
public:
  virtual spectrum at_point(const Vec3& pos) const = 0;
};

class SolidColor : public Texture
{
public:
  SolidColor(spectrum x = spectrum::zero) : color(x) {}

  spectrum at_point(const Vec3& pos) const override
  {
    return color;
  }
  
private:
  spectrum color;
};
