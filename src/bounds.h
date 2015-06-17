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
    AABB() { }
    AABB(const Vec3& min_, const Vec3& max_) : min(min_), max(max_) { }

    Vec3 size() const { return max - min; }

    static AABB box_union(const AABB& a, const AABB& b);

    scalar surface_area() const;
    bool hits_plane(int axis, scalar val) const
    {
      return min[axis] <= val && val <= max[axis];
    };

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

class Bounded
{
public:
  virtual bounds::AABB get_bounding_box() const = 0;
};
