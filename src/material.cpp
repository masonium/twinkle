#include "material.h"
#include "bsdf.h"
#include <memory>

using std::make_shared;

////////////////////////////////////////////////////////////////////////////////

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

Vec3 RoughMaterial::sample_bsdf(const Vec3& incoming, Sampler& sampler,
                                scalar& p, scalar& reflectance) const
{
  return brdf->sample(incoming, sampler, p, reflectance);
}

spectrum RoughMaterial::texture_at_point(const Intersection& isect) const
{
  return texture->at_point(isect);
}

////////////////////////////////////////////////////////////////////////////////

RoughColorMaterial::RoughColorMaterial(scalar roughness, const spectrum& color) :
  RoughMaterial(roughness, make_shared<SolidColor>(color))
{

}

//////////////////////////////////////////////////////////////////////////////////////////

scalar MirrorMaterial::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  return 0.0;
}

Vec3 MirrorMaterial::sample_bsdf(const Vec3& incoming, Sampler& sampler,
                                scalar& p, scalar& reflectance) const
{
  return brdf->sample(incoming, sampler, p, reflectance);
}

spectrum MirrorMaterial::texture_at_point(const Intersection& isect) const
{
  return spectrum{1.0};
}

////////////////////////////////////////////////////////////////////////////////

GlassMaterial::GlassMaterial(scalar refr_in, scalar refr_out) : 
  nr_inside(refr_in), nr_outside(refr_out)
{
  
}

scalar GlassMaterial::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  return 0.0;
}

Vec3 GlassMaterial::sample_bsdf(const Vec3& incoming, Sampler& sampler,
                                scalar& p, scalar& reflectance) const
{
  scalar n1 = nr_outside;
  scalar n2 = nr_inside;
  auto normal = Vec3::z_axis;
  if (incoming.z < 0)
  {
    std::swap<scalar>(n1, n2);
    normal *= -1;
  }
  scalar frpdf = fresnel_reflectance(incoming, normal, n1, n2);
  scalar samp = sampler.sample_1d();

  if (samp < frpdf)
  {
    // reflect
    p = frpdf;
    reflectance = frpdf * fabs(1 / incoming.z);
    return incoming.reflect_over(normal);
  }
  else
  {
    // transmit
    p = 1 - frpdf;
    reflectance = (1 - frpdf) * fabs(1 / incoming.z);
    return refraction_direction(incoming, normal, n1, n2);
  }
}

spectrum GlassMaterial::texture_at_point(const Intersection& isect) const
{
  return spectrum{1.0};
}
