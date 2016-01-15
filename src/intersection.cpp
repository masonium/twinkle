#include "intersection.h"
#include "shape.h"
#include "geometry.h"

IntersectionView::IntersectionView(const Shape* s, const SubGeo subgeo_, const Vec3& pos) :
  position(pos), subgeo(subgeo_), shape(s)
{
  assert(s != nullptr);

  normal = s->geometry->normal(subgeo, position);
  if (s->geometry->is_differential())
    s->geometry->texture_coord(subgeo, position, normal, tc.u, tc.v);
  to_z = Mat33::rotate_to_z(normal);
  from_z = to_z.transpose();
}

IntersectionView::IntersectionView(const Shape* s, const SubGeo subgeo_, const Ray& r, scalar tval)
  : IntersectionView(s, subgeo_, r.evaluate(tval))
{
  t_ = tval;
  assert(t_ >= 0);
}
Intersection::Intersection(const Shape* s, const SubGeo subgeo, const Vec3& pos)
  : IntersectionView(s, subgeo, pos)
{
}
Intersection::Intersection(const Shape* s, const SubGeo subgeo, const Ray& r, scalar t_)
    : IntersectionView(s, subgeo, r, t_)
{
}

spectrum Intersection::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  return shape->material->reflectance(*this, to_z * incoming, to_z * outgoing);
}


MaterialSample Intersection::sample_bsdf(const Vec3& incoming, Sampler& sampler) const
{
  auto local_incoming = to_z * incoming;
  auto ms = shape->material->sample_bsdf(*this, local_incoming, sampler);
  ms.direction = from_z * ms.direction;
  return ms;
}

bool Intersection::is_emissive() const
{
  return shape->material->is_emissive(*this);
}

spectrum Intersection::emission() const
{
  return shape->material->emission(*this);
}
