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
  Transformed(Transformed* ptr, Transform tr);
  Transformed(const Geometry* ptr, Transform tr);

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

  const Geometry* underlying() const { return geometry; }

private:
  const Geometry* geometry;
  Transform tform;
};

shared_ptr<Transformed> rotate(const Geometry*, Vec3 axis, scalar angle);

shared_ptr<Transformed> translate(const Geometry*, Vec3 displacement);

shared_ptr<Transformed> scale(const Geometry*, Vec3 sf);
