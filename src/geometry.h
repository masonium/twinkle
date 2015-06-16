#pragma once

#include <memory>
#include "vec3.h"
#include "intersection.h"
#include "ray.h"
#include <limits>

using std::shared_ptr;
using std::numeric_limits;

class Primitive
{
public:
  /*
   * Finds the intersection of a ray with the given object, returning the simple
   * object that it actually intersected with. The t-value of the intersection
   * must be less than or equal to max_t. If there is no valid intersection, the
   * returned t-value must be < 0, and geom need not be assigned.
   */
  virtual scalar intersect(const Ray& r, scalar max_t, const Geometry*& geom) const = 0;

  virtual ~Primitive() { }
};

class Geometry
{
public:
  virtual Vec3 normal(const Vec3& point) const = 0;
  virtual Vec3 sample_shadow_ray_dir(const Intersection& isect,
                                     scalar r1, scalar r2) const = 0;

  virtual bool is_differential() const { return false; }

  /*
   * texture_coord must be implemented if is_differential is true
   */
  virtual void texture_coord(const Vec3& pos, const Vec3& normal,
                             scalar& u, scalar& v, Vec3& dpdu, Vec3& dpdv) const
  {
  }

  virtual ~Geometry() {}
};

class SimpleGeometry : public Geometry, public Primitive
{
public:
  /**
   * Subclasses can override this method and have the max_t aspect taken care of
   * by the following method. 
   */
  virtual scalar intersect(const Ray& r) const {
    return static_cast<scalar>(-1.0);
  }

  /**
   * Alternatively, subclasses can override the max_t version
   * directly. Typcially, only one of these two needs to be overriden.
   */
  virtual scalar intersect(const Ray& r, scalar max_t) const {
    scalar x = intersect(r);
    return x > max_t ? -1 : x;
  }

  scalar intersect(const Ray& r, scalar max_t, const Geometry*& geom) const override
  {
    geom = this;
    return intersect(r, max_t);
  }

  virtual ~SimpleGeometry() { }
};

  
