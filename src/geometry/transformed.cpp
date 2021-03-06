#include "transformed.h"

using std::make_shared;

Transformed::Transformed(const Geometry* ptr, Transform tr)
  : geometry(ptr), tform(tr)
{
  assert(geometry);
}

Transformed::Transformed(Transformed* ptr, Transform tr)
  : geometry(ptr->geometry), tform(tr * ptr->tform)
{
  assert(geometry);
}

scalar_fp Transformed::intersect(const Ray& r, scalar_fp max_t, SubGeo& geom) const
{
  return geometry->intersect(tform.inv_transform_ray(r), max_t, geom);
}
Vec3 Transformed::normal(SubGeo sub_geo, const Vec3& point) const
{
  return tform.transform_normal( geometry->normal( sub_geo, tform.inv_transform_point(point)));
}

bool Transformed::is_differential() const
{
  return geometry->is_differential();
}
void Transformed::texture_coord(SubGeo subgeo, const Vec3& pos, const Vec3& normal,
                                scalar& u, scalar& v) const
{
  geometry->texture_coord(subgeo, tform.inv_transform_point(pos),
                          tform.inv_transform_normal(normal),
                          u, v);
}

bounds::AABB Transformed::get_bounding_box() const
{
  const auto& ob = geometry->get_bounding_box();

  auto new_min = ob.center();
  auto new_max = new_min;
  for (auto i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j)
      for (int k = 0; k < 2; ++k)
      {
        Vec3 transformed_corner = tform.transform_point(
          Vec3(ob.bounds[i].x, ob.bounds[j].y, ob.bounds[k].z));

        new_min = min(new_min, transformed_corner);
        new_max = max(new_max, transformed_corner);

      }
  return bounds::AABB{new_min, new_max};
}

Transform Transformed::transformation() const
{
  return tform;
}

////////////////////////////////////////////////////////////////////////////////

/*
 * FIX: This specification was originally setup to merge flatten
 * transformations at construction. It's useful with auto or chaining, but
 * basically useless once scripiting is implemented. At that point, this should
 * be cleaned up.
 */

shared_ptr<Transformed> condensed(const Geometry* geom, const Transform& trans)
{
  auto tr = dynamic_cast<const Transformed*>(geom);
  return make_shared<Transformed>(tr ? tr : geom, trans);
}

shared_ptr<Transformed> rotate(const Geometry* geom, Vec3 axis, scalar angle)
{
  auto rot = Transform{Mat33::from_axis_angle(axis.normal(), angle), Vec3::zero};
  return condensed(geom, rot);
}

shared_ptr<Transformed> translate(const Geometry* geom, Vec3 displacement)
{
  return condensed(geom, Transform{Mat33::identity, displacement});
}

shared_ptr<Transformed> scale(const Geometry* geom, Vec3 scale_factors)
{
  auto scale = Transform{Mat33::from_diagonal(scale_factors), Vec3::zero};
  return condensed(geom, scale);
}
