#include "bounds.h"
#include "math_util.h"

namespace bounds
{
  scalar AABB::surface_area() const
  {
    auto s = size();
    return 2*(s.x * s.y + s.y * s.z + s.z * s.x);
  }

  AABB AABB::box_union(const AABB& a, const AABB& b)
  {
    return AABB(::min(a.min(), b.min()), ::max(a.max(), b.max()));
  }

  scalar AABB::intersect(const Ray& r, scalar max_t)
  {
    scalar t0, t1;
    if (!ray_box_intersection(r, bounds, t0, t1))
        return -1;;

    if (t0 > 0)
    {
      if (t0 < max_t)
        return t0;
      else
        return -1;
    }
    else if (0 < t1 && t1 < max_t)
      return t1;
    else
      return -1;
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
  scalar tmin, tmax, tymin, tymax, tzmin, tzmax;

  tmin = (bounds[ray.sign[0]].x - ray.position.x) * ray.inv_direction.x;
  tmax = (bounds[1-ray.sign[0]].x - ray.position.x) * ray.inv_direction.x;

  tymin = (bounds[ray.sign[0]].y - ray.position.y) * ray.inv_direction.y;
  tymax = (bounds[1-ray.sign[0]].y - ray.position.y) * ray.inv_direction.y;

  if (tmin > tymax || tymin > tmax)
    return false;

  tmin = std::max(tmin, tymin);
  tmax = std::min(tmax, tymax);

  tzmin = (bounds[ray.sign[0]].z - ray.position.z) * ray.inv_direction.z;
  tzmax = (bounds[1-ray.sign[0]].z - ray.position.z) * ray.inv_direction.z;

  if (tmin > tzmax || tzmin > tmax)
    return false;

  tmin = std::max(tmin, tzmin);
  tmax = std::min(tmax, tzmax);

  if (tmin < tmax)
  {
    t0 = tmin;
    t1 = tmax;
    return true;
  }
  else
    return false;
}
