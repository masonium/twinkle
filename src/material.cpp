#include "material.h"
#include "bsdf.h"
#include "util.h"
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

spectrum RoughMaterial::reflectance(const IntersectionView& isect,
                                    const Vec3& incoming, const Vec3& outgoing) const
{
  return brdf->reflectance(incoming, outgoing) * texture->at_point(isect);
}

Vec3 RoughMaterial::sample_bsdf(const IntersectionView& isect, const Vec3& incoming, Sampler& sampler,
                                scalar& p, spectrum& reflectance) const
{
  scalar refl;
  const auto v = brdf->sample(incoming, sampler, p, refl);
  reflectance = texture->at_point(isect) * refl;
  return v;
}

////////////////////////////////////////////////////////////////////////////////

RoughColorMaterial::RoughColorMaterial(scalar roughness, const spectrum& color) :
  RoughMaterial(roughness, make_shared<SolidColor>(color))
{

}

//////////////////////////////////////////////////////////////////////////////////////////

spectrum MirrorMaterial::reflectance(const IntersectionView&,
                                     const Vec3& UNUSED(incoming), const Vec3& UNUSED(outgoing)) const
{
  return spectrum::zero;
}

Vec3 MirrorMaterial::sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                                scalar& p, spectrum& reflectance) const
{
  scalar refl;
  const auto v = brdf.sample(incoming, sampler, p, refl);
  reflectance = spectrum{refl};
  return v;
}

////////////////////////////////////////////////////////////////////////////////

GlassMaterial::GlassMaterial(scalar refr_in, scalar refr_out) : 
  nr_inside(refr_in), nr_outside(refr_out)
{
  
}

spectrum GlassMaterial::reflectance(const IntersectionView&,
                                    const Vec3& UNUSED(incoming), const Vec3& UNUSED(outgoing)) const
{
  return spectrum::zero;
}

Vec3 GlassMaterial::sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                                scalar& p, spectrum& reflectance) const
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
    reflectance = spectrum{frpdf * std::abs(1 / incoming.z)};
    return incoming.reflect_over(normal);
  }
  else
  {
    // transmit
    p = 1 - frpdf;
    reflectance = spectrum{(1 - frpdf) * std::abs(1 / incoming.z)};
    return refraction_direction(incoming, normal, n1, n2);
  }
}

