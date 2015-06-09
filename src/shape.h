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
  Shape(shared_ptr<Geometry> geom, shared_ptr<Material> mat)
    : geometry(geom), material(mat)
  {
  }
  
  scalar intersect(const Ray& r, const Geometry*& geom) const
  {
    return geometry->intersect(r, geom);
  }

  bool is_emissive() const
  {
    return material->is_emissive();
  }

  spectrum emission(const Intersection& isect) const
  {
    return material->emission(isect);
  }
  
  Vec3 sample_shadow_ray_dir(const Intersection& isect, scalar r1, scalar r2) const
  {
    return geometry->sample_shadow_ray_dir(isect, r1, r2);
  }
  
  shared_ptr<Geometry> geometry;
  shared_ptr<Material> material;
};
