#pragma once

#include <memory>
#include "vec3.h"
#include "intersection.h"
#include "ray.h"

using std::shared_ptr;

class Primitive
{
public:
  virtual scalar intersect(const Ray& r, const Geometry*& geom) const = 0;

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
  virtual scalar intersect(const Ray& r) const = 0;

  scalar intersect(const Ray& r, const Geometry*& geom) const override
  {
    geom = this;
    return intersect(r);
  }

  virtual ~SimpleGeometry() { }
};

  
