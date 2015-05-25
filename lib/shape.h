#pragma once

#include "math_util.h"
#include "bsdf.h"
#include "spectrum.h"
#include "texture.h"

class Intersection;

class Geometry
{
public:
  virtual scalar intersect(const Ray& r) const = 0;
  virtual scalar shadow_intersect(const Ray& r) const = 0;
  virtual Vec3 normal(Vec3 point) const = 0;
  virtual Vec3 sample_shadow_ray_dir(const Intersection& isect,
                                     scalar r1, scalar r2) const = 0;
};

class PossibleEmissive
{
public:
  virtual spectrum emission() const = 0;
  virtual Vec3 sample_shadow_ray_dir(const Intersection& isect,
                                     scalar r1, scalar r2) const = 0;
};

class Shape : public PossibleEmissive
{
public:
  Shape(Geometry* geom, BRDF* ref, Texture* tex) : geometry(geom), brdf(ref), texture(tex)
  {
  }
  
  scalar intersect(const Ray& r) const
  {
    return geometry->intersect(r);
  }
  
  scalar shadow_intersect(const Ray& r) const
  {
    return geometry->shadow_intersect(r);
  }

  bool is_emissive() const
  {
    return brdf->is_emissive();
  }

  spectrum emission() const override
  {
    return brdf->emission();
  }

  Vec3 sample_shadow_ray_dir(const Intersection& isect, scalar r1, scalar r2) const
  {
    return geometry->sample_shadow_ray_dir(isect, r1, r2);
  }
  const Geometry* geometry;
  BRDF* brdf;
  Texture* texture;
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

  spectrum texture_at_point() const
  {
    return shape->texture->at_point(position);
  }

  Vec3 sample_brdf(const Vec3& incoming, scalar r1, scalar r2, scalar& p) const
  {
    return shape->brdf->sample(incoming, normal, r1, r2, p);
  }
  
  const Shape* shape;
  scalar t;
  Vec3 position, normal;
};
