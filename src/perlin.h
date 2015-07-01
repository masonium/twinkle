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

  scalar noise(scalar r) const;
  scalar noise(scalar x, scalar y) const;

  scalar noise(const Vec2& v) const
  {
    return noise(v.x, v.y);
  }

  scalar noise(scalar x, scalar y, scalar z) const;

  scalar noise(const Vec3& v) const
  {
    return noise(v.x, v.y, v.z);
  }

private:
  uint8_t perm_x[256], perm_y[256], perm_z[256];
  scalar grad_1d[256];
  Vec2 grad_2d[256];
  Vec3 grad_3d[16];

  static scalar vdot(const Vec2& vec, scalar u, scalar v)
  {
    return vec.x * u + vec.y * v;
  }

  static scalar vdot(const Vec3& vec, scalar u, scalar v, scalar w)
  {
    return vec.x * u + vec.y * v + vec.z * w;
  }
};
