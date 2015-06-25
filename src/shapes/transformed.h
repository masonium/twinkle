#pragma once

#include <memory>
#include "geometry.h"
#include "transform.h"

using std::shared_ptr;

class Transformed : public Geometry
{
public:
  Transformed(shared_ptr<Geometry> ptr, Transform tr);

  virtual scalar intersect(const Ray& r, scalar max_t, SubGeo& geo) const override;

  virtual Vec3 normal(SubGeo sub_geo, const Vec3& point) const override;

  virtual bool is_differential() const override;

  /*
   * texture_coord must be implemented if is_differential is true
   */
  virtual void texture_coord(SubGeo subgeo, const Vec3& pos, const Vec3& normal,
                             scalar& u, scalar& v) const override;

private:
  shared_ptr<Geometry> geometry;
  Transform tform;
};
