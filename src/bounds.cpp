#include "bounds.h"
#include "math_util.h"
#include <limits>

using std::numeric_limits;

namespace bounds
{
  const AABB AABB::infinite(Vec3{numeric_limits<scalar>::min()}, Vec3{numeric_limits<scalar>::max()});

  scalar AABB::surface_area() const
  {
    auto s = size();
    return 2*(s.x * s.y + s.y * s.z + s.z * s.x);
  }

  bool AABB::operator ==(const AABB& a) const
  {
    return bounds[0] == a.bounds[0] && bounds[1] == a.bounds[1];
  }

  bool AABB::is_bounded() const
  {
    return bounds[0].is_bounded() && bounds[1].is_bounded();
  }

  AABB AABB::box_union(const AABB& a, const AABB& b)
  {
    return AABB(::min(a.min(), b.min()), ::max(a.max(), b.max()));
  }

  bool AABB::intersect(const Ray& r, scalar& t0, scalar& t1) const
  {
    return ray_box_intersection(r, bounds, t0, t1);
  }

  scalar_fp AABB::intersect(const Ray& r, scalar_fp max_t) const
  {
    scalar t0, t1;
    if (!ray_box_intersection(r, bounds, t0, t1))
      return sfp_none;;

    if (t0 > 0)
    {
      return t0 < max_t ? scalar_fp{t0} : sfp_none;
    }
    else if (0 < t1 && t1 < max_t)
      return scalar_fp{t1};
    else
      return sfp_none;
  }

  using std::ostream;
  ostream& operator<<(ostream& out, const bounds::AABB& b)
  {
    out << "{" << b.min() << "} - {" << b.max() << "}";
    return out;
  }
}

bool ray_box_intersection(const Ray& ray, const Vec3 bounds[2],
                          scalar& t0, scalar& t1)
{
  scalar tymin, tymax, tzmin, tzmax;

  t0 = (bounds[ray.sign[0]].x - ray.position.x) * ray.inv_direction.x;
  t1 = (bounds[1-ray.sign[0]].x - ray.position.x) * ray.inv_direction.x;

  tymin = (bounds[ray.sign[1]].y - ray.position.y) * ray.inv_direction.y;
  tymax = (bounds[1-ray.sign[1]].y - ray.position.y) * ray.inv_direction.y;

  if (t0 > tymax || tymin > t1)
  {
    return false;
  }

  t0 = std::max(t0, tymin);
  t1 = std::min(t1, tymax);

  tzmin = (bounds[ray.sign[2]].z - ray.position.z) * ray.inv_direction.z;
  tzmax = (bounds[1-ray.sign[2]].z - ray.position.z) * ray.inv_direction.z;

  if (t0 > tzmax || tzmin > t1)
    return false;

  t0 = std::max(t0, tzmin);
  t1 = std::min(t1, tzmax);

  return t0 < t1;
}
