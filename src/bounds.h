#pragma once

#include "vec3.h"
#include <iostream>

namespace bounds
{
  using std::ostream;

  class Base
  {
  public:

  };


  class AABB : public Base
  {
  public:
    AABB(const Vec3& min_, const Vec3& max_) : min(min_), max(max_) { }

    Vec3 min, max;
  };

  ostream& operator<<(ostream& out, const bounds::AABB& b);

  class Sphere : public Base
  {
  public:
    Sphere(const Vec3& center_, scalar rad_) : center(center_), radius(rad_) { }

    Vec3 center;
    scalar radius;
  };
}