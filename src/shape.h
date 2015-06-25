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
  Shape(shared_ptr<Geometry> prim, shared_ptr<Material> mat)
    : geometry(prim), material(mat)
  {
  }
  
  scalar intersect(const Ray& r, scalar max_t, SubGeo& subgeo) const
  {
    return geometry->intersect(r, max_t, subgeo);
  }

  bool is_emissive() const
  {
    return material->is_emissive();
  }

  spectrum emission(const Intersection& isect) const
  {
    return material->emission(isect);
  }
  
  shared_ptr<Geometry> geometry;
  shared_ptr<Material> material;
};
