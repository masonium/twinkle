#pragma once

#include "ray.h"
#include "spectrum.h"
#include "sampler.h"
#include "mat33.h"
#include "vec2.h"
#include "geometry.h"

class Shape;
struct MaterialSample;

/**
 * An IntersectionView represents the results of intersection a ray with the
 * scene. It stores relevant local information about the intersection, in
 * addition to the shape and sub-shape.
 */
class IntersectionView
{
public:
  scalar t() const
  {
    return t_;
  }

  const Shape* get_shape_for_id() const { return shape; }

  Vec3 position, normal;
  Vec2 tc;
  Vec3 dpdu, dpdv;

protected:
  IntersectionView(const Shape* s, const SubGeo subgeo, const Vec3& pos);
  IntersectionView(const Shape* s, const SubGeo subgeo, const Ray& r, scalar t_);

  const SubGeo subgeo;
  const Shape* shape;
  Mat33 to_z;
  Mat33 from_z;
  scalar t_;
};

/**
 * An intersection also has member functions to evaluate local shading
 * information.
 */
class Intersection : public IntersectionView
{
public:
  Intersection(const Shape* s, const SubGeo subgeo, const Vec3& pos);
  Intersection(const Shape* s, const SubGeo subgeo, const Ray& r, scalar t_);

  spectrum reflectance(const Vec3& incoming, const Vec3& outgoing) const;

  MaterialSample sample_bsdf(const Vec3& incoming, Sampler& sampler) const;


  bool is_emissive() const;

  spectrum emission() const;
};
