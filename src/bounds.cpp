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
    return ray_box_intersection(r, bounds, max_t);
  }

  using std::ostream;
  ostream& operator<<(ostream& out, const bounds::AABB& b)
  {
    out << "{" << b.min() << "} - {" << b.max() << "}";
    return out;
  }
}
