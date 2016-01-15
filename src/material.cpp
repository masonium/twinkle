#include "material.h"
#include "textures.h"
#include "bsdf.h"
#include "util.h"
#include <sstream>
#include <memory>

using std::make_shared;

////////////////////////////////////////////////////////////////////////////////

RoughMaterial::RoughMaterial(scalar roughness_, const Texture* tex_) :
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

scalar RoughMaterial::pdf(const Vec3& incoming,
                          const Vec3& outgoing) const
{
  return brdf->pdf(incoming, outgoing);
}

////////////////////////////////////////////////////////////////////////////////

RoughColorMaterial::RoughColorMaterial(scalar roughness, const spectrum& color) :
  RoughMaterial(roughness, make_entity<SolidColor>(color).get()), c(color)
{

}

std::string RoughColorMaterial::to_string() const
{
  std::ostringstream s;
  s << "RCMat(" << c << ")";
  return s.str();
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

scalar MirrorMaterial::pdf(const Vec3& incoming,
                           const Vec3& outgoing) const
{
  return brdf.pdf(incoming, outgoing);
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
  scalar az = std::abs(incoming.z);
  if (az < 0.0001)
  {
    p = 0;
    reflectance = spectrum{0};
    return Vec3::zero;
  }

  if (incoming.z < 0)
  {
    std::swap<scalar>(n1, n2);
    normal *= -1;
  }
  scalar fr = fresnel_reflectance(incoming, normal, n1, n2);
  scalar refl_prob = fr;
  scalar samp = sampler.sample_1d();

  if (samp < refl_prob)
  {
    // reflect
    p = refl_prob;
    reflectance = spectrum{fr / az};
    return incoming.reflect_over(normal);
  }
  else
  {
    // transmit
    p = 1 - refl_prob;
    reflectance = spectrum{(1 - fr) / az};
    return refraction_direction(incoming, normal, n1, n2);
  }
}

scalar GlassMaterial::pdf(const Vec3& UNUSED(incoming),
                          const Vec3& UNUSED(outgoing)) const
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
EmissiveMaterial::EmissiveMaterial(const Texture* tex) : 
  texture(tex)
{
}

spectrum EmissiveMaterial::reflectance(const IntersectionView&, 
                                       const Vec3&, const Vec3&) const
{
  return spectrum::zero;
}

Vec3 EmissiveMaterial::sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                                   scalar& p, spectrum& reflectance) const
{
  p = 0;
  reflectance = spectrum{0.0};
  return Vec3::zero;
}

scalar EmissiveMaterial::pdf(const Vec3&, const Vec3&) const
{
  return 0.0;
}

spectrum EmissiveMaterial::emission(const IntersectionView& isect) const
{
  return texture->at_point(isect);
}
  
