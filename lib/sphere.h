#pragma once

#include "math_util.h"
#include "shape.h"

class Sphere : public Geometry
{
public:
  Sphere(Vec3 pos, scalar r) : position(pos), radius(r), radius2(r*r)
  {
  }
  
  scalar intersect(const Ray& r) const override;
  
  Vec3 normal(Vec3 point) const override;

  Vec3 sample_shadow_ray_dir(const Intersection& isect,
                               scalar r1, scalar r2) const override;

  bool is_differential() const override { return true; }
  
  void texture_coord(const Vec3& pos, const Vec3& normal,
                     scalar& u, scalar& v, Vec3& dpdu, Vec3& dpdv) const override;

  Vec3 position;
  scalar radius, radius2;
};
