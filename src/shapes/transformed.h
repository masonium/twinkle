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
  Transformed(shared_ptr<Geometry> ptr, Transform tr);

  virtual scalar intersect(const Ray& r, scalar max_t, SubGeo& geo) const override;

  virtual Vec3 normal(SubGeo sub_geo, const Vec3& point) const override;

  virtual bool is_differential() const override;

  /*
   * texture_coord must be implemented if is_differential is true
   */
  virtual void texture_coord(SubGeo subgeo, const Vec3& pos, const Vec3& normal,
                             scalar& u, scalar& v) const override;

  Transform transformation() const;

private:
  shared_ptr<Geometry> geometry;
  Transform tform;
};

shared_ptr<Transformed> rotate(shared_ptr<Transformed>, Vec3 axis, scalar angle);
shared_ptr<Transformed> rotate(shared_ptr<Geometry>, Vec3 axis, scalar angle);

shared_ptr<Transformed> translate(shared_ptr<Transformed>, Vec3 displacement);
shared_ptr<Transformed> translate(shared_ptr<Geometry>, Vec3 displacement);
