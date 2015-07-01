#pragma once

#include "texture.h"
#include "vec2.h"
#include "vec3.h"
#include <memory.h>

class PerlinNoise
{
public:
  PerlinNoise();
  static shared_ptr<PerlinNoise> get_instance();

  scalar noise(scalar r);
  scalar noise(scalar x, scalar y) const;
  scalar noise(scalar x, scalar y, scalar z);
  scalar noise(Vec3 v);

private:
  uint8_t perm_x[256], perm_y[256], perm_z[256];
  scalar grad_1d[];
  Vec2 grad_2d[256];
  Vec3 grad_3d[16];

  static scalar vdot(Vec2 vec, scalar u, scalar v)
  {
    return vec.x * u + vec.y * v;
  }

  static scalar scurve(scalar t);
};
