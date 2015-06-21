#include "intersection.h"
#include "shape.h"
#include "geometry.h"

Intersection::Intersection(const Shape* s, const Geometry* geom, const Ray& r, scalar t_) :
  t(t_), position(r.evaluate(t)), geometry(geom), shape(s)
{
  if (s != nullptr)
  {
    normal = geometry->normal(position);
    if (geometry->is_differential())
      geometry->texture_coord(position, normal, u, v, dpdu, dpdv);
    to_z = Mat33::rotate_to_z(normal);
    from_z = to_z.transpose();
  }

}

Intersection::operator bool() const 
{
  return valid();
}
  
bool Intersection::valid() const
{
  return shape != nullptr;
}

scalar Intersection::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  return shape->material->reflectance(to_z * incoming, to_z * outgoing);
}

spectrum Intersection::texture_at_point() const
{
  return shape->material->texture_at_point(*this);
}

Vec3 Intersection::sample_bsdf(const Vec3& incoming, const Sample2D& sample,
                               scalar& p, scalar& reflectance) const
{
  auto local_incoming = to_z * incoming;
  auto local_outgoing = shape->material->sample_bsdf(local_incoming, sample, p, reflectance);
  auto outgoing = from_z * local_outgoing;
  return outgoing;
}

bool Intersection::is_emissive() const
{
  return shape->material->is_emissive();
}

spectrum Intersection::emission() const
{
  return shape->material->emission(*this);
}
