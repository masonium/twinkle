#include "material.h"
#include "bsdf.h"
#include <memory>

using std::make_shared;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RoughMaterial::RoughMaterial(scalar roughness_, shared_ptr<Texture> tex_) :
  texture(tex_)
{
  if (roughness_ > 0)
    brdf = unique_ptr<BRDF>(new OrenNayar(1.0, roughness_));
  else
    brdf = unique_ptr<BRDF>(new Lambertian(1.0));
}

scalar RoughMaterial::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  return brdf->reflectance(incoming, outgoing);
}

Vec3 RoughMaterial::sample_bsdf(const Vec3& incoming, const Sample2D& sample,
		   scalar& p, scalar& reflectance) const
{
  return brdf->sample(incoming, sample, p, reflectance);
}

spectrum RoughMaterial::texture_at_point(const Intersection& isect) const
{
  return texture->at_point(isect);
}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RoughColorMaterial::RoughColorMaterial(scalar roughness, const spectrum& color) :
  RoughMaterial(roughness, make_shared<SolidColor>(color))
{

}
