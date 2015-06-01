#pragma once

#include "ray.h"
#include "spectrum.h"

class Shape;

class Intersection
{
public:
  Intersection(const Shape* s, const Ray& r, scalar t_);
  operator bool() const;
  
  bool valid() const;

  spectrum texture_at_point() const;
  Vec3 sample_brdf(const Vec3& incoming, scalar r1, scalar r2, 
                   scalar& p, scalar& reflectance) const;
  
  const Shape* shape;
  scalar t;
  Vec3 position, normal;
  scalar u, v;
  Vec3 dpdu, dpdv;
};
