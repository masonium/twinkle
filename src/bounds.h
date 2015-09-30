#pragma once

#include "vec3.h"
#include "ray.h"
#include "geometry/isect_util.h"
#include <iostream>

bool ray_box_intersection(const Ray& ray, const Vec3 bounds[2],
                          scalar& t0, scalar& t1);

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
    AABB(const Vec3& min_, const Vec3& max_) : bounds{min_, max_} { }

    /**
     * size of the bounding box
     */
    Vec3 size() const { return bounds[1] - bounds[0]; }

    Vec3 center() const { return (bounds[0] + bounds[1]) * 0.5; }

    /**
     * Return the smallest boudning box containing both boxes.
     */
    static AABB box_union(const AABB& a, const AABB& b);

    const Vec3& min() const { return bounds[0]; }
    Vec3& min() { return bounds[0]; }

    const Vec3& max() const { return bounds[1]; }
    Vec3& max() { return bounds[1]; }

    bool operator ==(const AABB&) const;

    /**
     * Return true iff all of the compenents are bounded. A component is
     * unbounded iff it is equal to scalar_max or scalar_min
     */
    bool is_bounded() const;

    /**
     * Return the surface area of this box.
     */
    scalar surface_area() const;

    /**
     * Return true if this (closed) box intersects with the axis-aligned plane
     * specified by axis and val.
     */
    bool hits_plane(int axis, scalar val) const
    {
      return bounds[0][axis] <= val && val <= bounds[1][axis];
    };

    /**
     * Intersection of this box with a ray.
     */
    scalar_fp intersect(const Ray& r, scalar_fp max_t) const;
    bool intersect(const Ray& r, scalar& t0, scalar& t1) const;

    static const AABB infinite;

    Vec3 bounds[2];
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
