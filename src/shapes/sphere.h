#pragma once

#include "math_util.h"
#include "shape.h"
#include "bounds.h"

class Sphere : public SimpleGeometry
{
public:
  Sphere(Vec3 pos, scalar r) : position(pos), radius(r), radius2(r*r)
  {
  }
  
  scalar_fp intersect(const Ray& r) const override;
  
  Vec3 normal(SubGeo, const Vec3& point) const override;

  bool is_differential() const override { return true; }
  
  void texture_coord(SubGeo, const Vec3& pos, const Vec3& normal,
                     scalar& u, scalar& v) const override;

  bool is_bounded() const override { return true; }

  bounds::AABB get_bounding_box() const override;

  Vec3 position;
  scalar radius, radius2;
};

