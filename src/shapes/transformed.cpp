#include "transformed.h"

using std::make_shared;

Transformed::Transformed(shared_ptr<Geometry> ptr, Transform tr)
  : geometry(ptr), tform(tr)
{
}

Transformed::Transformed(shared_ptr<Transformed> ptr, Transform tr)
  : geometry(ptr->geometry), tform(tr * ptr->tform)
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

Transform Transformed::transformation() const
{
  return tform;
}

////////////////////////////////////////////////////////////////////////////////
shared_ptr<Transformed> rotate(shared_ptr<Transformed> geom, Vec3 axis, scalar angle)
{
  auto rot = Transform{Mat33::from_axis_angle(axis.normal(), angle), Vec3::zero};
  return make_shared<Transformed>(geom, rot);
}

shared_ptr<Transformed> rotate(shared_ptr<Geometry> geom, Vec3 axis, scalar angle)
{
  return rotate(make_shared<Transformed>(geom, Transform()), axis, angle);
}

shared_ptr<Transformed> translate(shared_ptr<Transformed> geom, Vec3 displacement)
{
  auto trans = Transform{Mat33::identity, displacement};
  return make_shared<Transformed>(geom, trans);

}

shared_ptr<Transformed> translate(shared_ptr<Geometry> geom, Vec3 displacement)
{
  return translate(make_shared<Transformed>(geom, Transform()), displacement);
}
