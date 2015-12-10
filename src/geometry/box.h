#pragma once

#include "math_util.h"
#include "shape.h"
#include "bounds.h"

class Box : public SimpleGeometry
{
public:
  Box(const bounds::AABB& box) : _bounds(box)
  {
  }

  scalar_fp intersect(const Ray& r, scalar_fp max_t) const override;

  Vec3 normal(SubGeo, const Vec3& point) const override;

  bool is_differential() const override { return false; }

  void texture_coord(SubGeo, const Vec3& pos, const Vec3& normal,
                     scalar& u, scalar& v) const override;

  bool is_bounded() const override { return true; }

  bounds::AABB get_bounding_box() const override;

  std::string to_string() const override;

  bounds::AABB _bounds;
};
