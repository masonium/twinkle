#include "transformed.h"

Transformed::Transformed(shared_ptr<Geometry> ptr, Transform tr)
  : geometry(ptr), tform(tr)
{
}

scalar Transformed::intersect(const Ray& r, scalar max_t, SubGeo& geom) const
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
