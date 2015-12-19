#pragma once

#include "base.h"
#include "spectrum.h"
#include "vec2.h"
#include "vec3.h"

class IntersectionView;

class Texture : public Base
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
