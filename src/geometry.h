#pragma once

#include <memory>
#include "vec3.h"
#include "intersection.h"
#include "ray.h"

using std::shared_ptr;

class Geometry
{
public:
  virtual scalar intersect(const Ray& r) const = 0;
  virtual Vec3 normal(Vec3 point) const = 0;
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
};

class ReversedGeometry : public Geometry
{
public:
  ReversedGeometry(shared_ptr<Geometry> geom) : geometry(geom) {}

  scalar intersect(const Ray& r) const override
  {
    return geometry->intersect(r);
  }

  Vec3 normal(Vec3 point) const override
  {
    return -geometry->normal(point);
  }

  Vec3 sample_shadow_ray_dir(const Intersection& isect,
                             scalar r1, scalar r2) const override
  {
    return sample_shadow_ray_dir(isect, r1, r2);
  }

  bool is_differential() const override { return geometry->is_differential(); }

  void texture_coord(const Vec3& pos, const Vec3& normal,
                     scalar& u, scalar& v, Vec3& dpdu, Vec3& dpdv) const override
  {
    return geometry->texture_coord(pos, -normal, u, v, dpdu, dpdv);
  }

private:
  shared_ptr<Geometry> geometry;
};
