#pragma once

#include "ray.h"
#include "spectrum.h"
#include "sampler.h"

class Shape;

class Intersection
{
public:
  Intersection(const Shape* s, const Ray& r, scalar t_);
  operator bool() const;
  
  bool valid() const;

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const;
  
  spectrum texture_at_point() const;
  Vec3 sample_bsdf(const Vec3& incoming, const Sample2D& sample,
                   scalar& p, scalar& reflectance) const;
  
  const Shape* shape;
  scalar t;
  Vec3 position, normal;
  scalar u, v;
  Vec3 dpdu, dpdv;
};
