#pragma once

#include "math_util.h"
#include "bsdf.h"

class Geometry;

class Geometry
{
public:
  virtual scalar intersect(const Ray& r) const = 0;
  virtual Vec3 normal(Vec3 point) const = 0;
};


class Shape
{
public:
  Shape(Geometry* geom, BRDF* ref) : geometry(geom), brdf(ref)
  {
  }
  
  scalar intersect(const Ray& r) const
  {
    return geometry->intersect(r);
  }

  const Geometry* geometry;
  BRDF* brdf;
};

class Intersection
{
public:
  Intersection(const Shape* s, const Ray& r, scalar t_) :
    shape(s), t(t_), position(r.evaluate(t))
  {
    if (s != nullptr)
      normal = s->geometry->normal(position);
  }

  bool valid() const
  {
    return shape != nullptr;
  }
  
  const Shape* shape;
  scalar t;
  Vec3 position, normal;
};
