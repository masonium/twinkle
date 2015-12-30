#pragma once

#include <memory>
#include <sstream>
#include "spectrum.h"
#include "ray.h"
#include "material.h"
#include "geometry.h"
#include "util/optional.h"


using std::shared_ptr;

class Intersection;

class Shape : public Base
{
public:
  Shape(const Geometry* prim, const Material* mat)
    : geometry(prim), material(mat)
  {
  }
  
  scalar_fp intersect(const Ray& r, scalar_fp max_t, SubGeo& subgeo) const
  {
    return geometry->intersect(r, max_t, subgeo);
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

  std::string to_string() const
  {
    std::ostringstream s;
    s << "Shape(" << geometry->to_string() << "; " << material->to_string() << ")";
    return s.str();
  }
  
  const Geometry* geometry;
  const Material* material;
};
