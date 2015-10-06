#pragma once

#include "ray.h"
#include "spectrum.h"
#include "sampler.h"
#include "mat33.h"
#include "vec2.h"
#include "geometry.h"

class Shape;

/**
 * An intersection represents the results of intersection a ray with the
 * scene. It stores relevant local information about the intersection, in
 * addition to the shape and sub-shape.
 */
class Intersection
{
public:
  Intersection(const Shape* s, const SubGeo subgeo, const Vec3& pos);
  Intersection(const Shape* s, const SubGeo subgeo, const Ray& r, scalar t_);

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const;
  
  spectrum texture_at_point() const;
  Vec3 sample_bsdf(const Vec3& incoming, Sampler& sampler,
                   scalar& p, scalar& reflectance) const;

  scalar t() const
  {
    return t_;
  }

  bool is_emissive() const;

  spectrum emission() const;

  const Shape* get_shape_for_id() const { return shape; }

  Vec3 position, normal;
  Vec2 tc;
  Vec3 dpdu, dpdv;

private:
  const SubGeo subgeo;
  const Shape* shape;
  Mat33 to_z;
  Mat33 from_z;
  scalar t_;
};
