#pragma once

#include <memory>
#include "math_util.h"
#include "spectrum.h"
#include "ray.h"
#include "material.h"

using std::shared_ptr;

class Intersection;

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

class Shape
{
public:
  Shape(shared_ptr<Geometry> geom, shared_ptr<Material> mat)
    : geometry(geom), material(mat)
  {
  }
  
  scalar intersect(const Ray& r) const
  {
    return geometry->intersect(r);
  }

  bool is_emissive() const
  {
    return material->is_emissive();
  }

  spectrum emission(const Intersection& isect) const
  {
    return material->emission(isect);
  }

  bool is_differential() const
  {
    return geometry->is_differential();
  }
  
  Vec3 sample_shadow_ray_dir(const Intersection& isect, scalar r1, scalar r2) const
  {
    return geometry->sample_shadow_ray_dir(isect, r1, r2);
  }
  
  shared_ptr<Geometry> geometry;
  shared_ptr<Material> material;
};
