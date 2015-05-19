#pragma once

#include "math_util.h"


class BRDF
{
public:
  virtual scalar reflectance(Vec3 incoming, Vec3 outgoing, Vec3 normal) const = 0;
};

class Diffuse : public BRDF
{
public:
  explicit Diffuse( scalar r_ ) : r(r_ / (2.0 * PI))
  {
  }
  
  scalar reflectance(Vec3 incoming, Vec3 outgoing, Vec3 normal) const override
  {
    if (incoming.dot(normal) < 0)
      return 0.0f;
    return r;
  }

  scalar r;
};

