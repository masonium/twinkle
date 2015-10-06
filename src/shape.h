#pragma once

#include <memory>
#include "spectrum.h"
#include "ray.h"
#include "material.h"
#include "geometry.h"
#include "util/optional.h"

using std::shared_ptr;

class Intersection;

class Shape
{
public:
  Shape(shared_ptr<const Geometry> prim, shared_ptr<const Material> mat)
    : geometry(prim), material(mat)
  {
  }
  
  scalar_fp intersect(const Ray& r, scalar_fp max_t, SubGeo& subgeo) const
  {
    return geometry->intersect(r, max_t, subgeo);
  }

  bool is_emissive(const IntersectionView& isect) const
  {
    return material->is_emissive(isect);
  }

  spectrum emission(const IntersectionView& isect) const
  {
    return material->emission(isect);
  }

  bool is_bounded() const
  {
    return geometry->is_bounded();
  }

  bounds::AABB get_bounding_box() const
  {
    return geometry->get_bounding_box();
  }
  
  shared_ptr<const Geometry> geometry;
  shared_ptr<const Material> material;
};
