#include "intersection.h"
#include "shape.h"

Intersection::Intersection(const Shape* s, const Ray& r, scalar t_) :
  shape(s), t(t_), position(r.evaluate(t))
{
  if (s != nullptr)
  {
    normal = s->geometry->normal(position);
    if (s->is_differential())
      s->geometry->texture_coord(position, normal, u, v, dpdu, dpdv);
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
  return 0;
}

spectrum Intersection::texture_at_point() const
{
  return shape->material->texture_at_point(*this);
}

Vec3 Intersection::sample_bsdf(const Vec3& incoming, const Sample2D& sample,
                               scalar& p, scalar& reflectance) const
{
  return shape->material->sample_bsdf(incoming, sample, p, reflectance);
}
