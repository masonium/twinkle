#include "box.h"
#include "util.h"
#include <sstream>

using std::move;

scalar_fp Box::intersect(const Ray& r, scalar_fp max_t) const
{
  return _bounds.intersect(r, max_t);
}

Vec3 Box::normal(SubGeo UNUSED(geo), const Vec3& point) const
{
  auto min_side = (point - _bounds.min()).abs();

  auto norm = Vec3::zero;
  if (min_side.min() < EPSILON)
  {
    norm[min_side.min_element()] = -1.0;
  }
  else
  {
    auto max_side = (point - _bounds.max()).abs();
    norm[max_side.min_element()] = 1.0;
  }
  return norm;
}

void Box::texture_coord(SubGeo UNUSED(geo), const Vec3& UNUSED(pos), const Vec3& normal,
                           scalar& u, scalar& v) const
{
}

bounds::AABB Box::get_bounding_box() const
{
  return _bounds;
}

std::string Box::to_string() const
{
  std::ostringstream s;
  s << "Box(" << _bounds <<")";
  return s.str();
}
