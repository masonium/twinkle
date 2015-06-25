#pragma once

#include <memory>
#include "vec3.h"
#include "ray.h"
#include <limits>

using std::shared_ptr;
using std::numeric_limits;

using SubGeo = uint64_t;

class Geometry
{
public:
  virtual scalar intersect(const Ray& r, scalar max_t, SubGeo& geom) const = 0;

  virtual Vec3 normal(SubGeo sub_geo, const Vec3& point) const = 0;

  virtual bool is_differential() const { return false; }

  /*
   * texture_coord must be implemented if is_differential is true
   */
  virtual void texture_coord(SubGeo subgeo, const Vec3& pos, const Vec3& normal,
                             scalar& u, scalar& v) const
  {
  }

  virtual ~Geometry() {}
};

class SimpleGeometry : public Geometry
{
public:
  /**
   * Subclasses can override this method and have the max_t aspect taken care of
   * by the following method. 
   */
  virtual scalar intersect(const Ray& r) const {
    return static_cast<scalar>(-1.0);
  }

  /**
   * Alternatively, subclasses can override the max_t version
   * directly. Typcially, only one of these two needs to be overriden.
   */
  virtual scalar intersect(const Ray& r, scalar max_t) const {
    scalar x = intersect(r);
    return x > max_t ? -1 : x;
  }

  scalar intersect(const Ray& r, scalar max_t, SubGeo& geom) const override
  {
    return intersect(r, max_t);
  }

  virtual ~SimpleGeometry() { }
};

  
