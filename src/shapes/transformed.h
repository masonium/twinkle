#pragma once

#include <memory>
#include "geometry.h"
#include "transform.h"

using std::shared_ptr;

/**
 * Transformed represents a geometry that is modified by a an affine
 * transformation.
 */
class Transformed : public Geometry
{
public:
  Transformed(shared_ptr<Transformed> ptr, Transform tr);
  Transformed(shared_ptr<const Geometry> ptr, Transform tr);

  virtual scalar_fp intersect(const Ray& r, scalar_fp max_t, SubGeo& geo) const override;

  virtual Vec3 normal(SubGeo sub_geo, const Vec3& point) const override;

  virtual bool is_differential() const override;

  /*
   * texture_coord must be implemented if is_differential is true
   */
  virtual void texture_coord(SubGeo subgeo, const Vec3& pos, const Vec3& normal,
                             scalar& u, scalar& v) const override;

  bool is_bounded() const override { return geometry->is_bounded(); }

  bounds::AABB get_bounding_box() const override;

  Transform transformation() const;

  shared_ptr<const Geometry> underlying() const { return geometry; }

private:
  shared_ptr<const Geometry> geometry;
  Transform tform;
};

shared_ptr<Transformed> rotate(shared_ptr<Transformed>, Vec3 axis, scalar angle);
shared_ptr<Transformed> rotate(shared_ptr<Geometry>, Vec3 axis, scalar angle);

shared_ptr<Transformed> translate(shared_ptr<Transformed>, Vec3 displacement);
shared_ptr<Transformed> translate(shared_ptr<Geometry>, Vec3 displacement);

shared_ptr<Transformed> scale(shared_ptr<Transformed>, Vec3 sf);
shared_ptr<Transformed> scale(shared_ptr<Geometry>, Vec3 sf);
