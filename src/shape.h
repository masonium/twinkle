#pragma once

#include <memory>
#include "math_util.h"
#include "spectrum.h"
#include "ray.h"
#include "material.h"
#include "geometry.h"

using std::shared_ptr;

class Intersection;

class Shape
{
public:
  Shape(shared_ptr<Intersectable> prim, shared_ptr<Material> mat)
    : intersectable(prim), material(mat)
  {
  }
  
  scalar intersect(const Ray& r, scalar max_t, const Geometry*& geom) const
  {
    return intersectable->intersect(r, max_t, geom);
  }

  bool is_emissive() const
  {
    return material->is_emissive();
  }

  spectrum emission(const Intersection& isect) const
  {
    return material->emission(isect);
  }
  
  // Vec3 sample_shadow_ray_dir(const Intersection& isect, scalar r1, scalar r2) const
  // {
  //   return geometry->sample_shadow_ray_dir(isect, r1, r2);
  // }
  
  shared_ptr<Intersectable> intersectable;
  shared_ptr<Material> material;
};
