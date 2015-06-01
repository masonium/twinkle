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

spectrum Intersection::texture_at_point() const
{
  return shape->texture->at_point(*this);
}

Vec3 Intersection::sample_brdf(const Vec3& incoming, scalar r1, scalar r2, 
                 scalar& p, scalar& reflectance) const
{
  return shape->brdf->sample(incoming, normal, r1, r2, p, reflectance);
}
