#pragma once

#include <limits>
#include <memory>
#include <util/manager.h>
#include "base.h"
#include "util.h"
#include "geometry/isect_util.h"
#include "vec3.h"
#include "ray.h"
#include "bounds.h"

using std::shared_ptr;
using std::numeric_limits;

using SubGeo = int64_t;
const SubGeo SUBGEO_NONE = -1;

class Geometry : public Base
{
public:
  virtual scalar_fp intersect(const Ray& r, scalar_fp max_t, SubGeo& geom) const = 0;

  virtual Vec3 normal(SubGeo sub_geo, const Vec3& point) const = 0;

  virtual bool is_differential() const { return false; }

  /*
   * texture_coord must be implemented if is_differential is true
   */
  virtual void texture_coord(SubGeo subgeo, const Vec3& pos, const Vec3& normal,
                             scalar& u, scalar& v) const;

  virtual bool is_bounded() const { return false; }

  virtual bounds::AABB get_bounding_box() const { return bounds::AABB::infinite; }

  virtual string to_string() const override { return "Geometry{}"; }

  virtual ~Geometry() {}
};

class SimpleGeometry : public Geometry
{
public:
  /**
   * Subclasses can override this method and have the max_t aspect taken care of
   * by the following method. 
   */
  virtual scalar_fp intersect(const Ray& UNUSED(r)) const {
    return sfp_none;
  }

  /**
   * Alternatively, subclasses can override the max_t version
   * directly. Typcially, only one of these two needs to be overriden.
   */
  virtual scalar_fp intersect(const Ray& r, scalar_fp max_t) const {
    auto x = intersect(r);
    return x < max_t ? x : sfp_none;
  }

  scalar_fp intersect(const Ray& r, scalar_fp max_t, SubGeo& UNUSED(geom)) const override
  {
    return intersect(r, max_t);
  }

  virtual ~SimpleGeometry() { }
};

