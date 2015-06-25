#pragma once

#include "geometry.h"

class Plane : public SimpleGeometry
{
public:
  // N*p + d = 0;
  Plane(const Vec3& normal, scalar offset);

  scalar intersect(const Ray& r) const override;
  Vec3 normal(SubGeo geo, const Vec3& point) const override;

  bool is_differential() const override { return true; }

  void texture_coord(SubGeo geo, const Vec3& pos, const Vec3& normal,
                     scalar& u, scalar& v) const override;

private:
  Vec3 N;
  Vec3 U, V;
  scalar d;
};
