#include "material.h"
#include "textures.h"
#include "util.h"
#include <sstream>
#include <memory>

using std::make_shared;

MaterialSample MaterialSample::invalid{Vec3::zero, 0, spectrum::zero};

MaterialSample::MaterialSample(const Vec3& d ,scalar p, const spectrum& s)
  : direction(d), prob(p), reflectance(s)
{

}

MaterialSample::MaterialSample(const BSDFSample& bs, const spectrum& s)
  : direction(bs.direction), prob(bs.prob), reflectance(s * bs.reflectance)
{
}

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

MaterialSample RoughMaterial::sample_bsdf(
  const IntersectionView& isect, const Vec3& incoming, Sampler& sampler) const
{
  return MaterialSample{brdf->sample(incoming, sampler), texture->at_point(isect)};
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

MaterialSample MirrorMaterial::sample_bsdf(
  const IntersectionView&, const Vec3& incoming, Sampler& sampler) const
{
  return MaterialSample{brdf.sample(incoming, sampler), spectrum{1.0}};

}

scalar MirrorMaterial::pdf(const Vec3& incoming,
                           const Vec3& outgoing) const
{
  return brdf.pdf(incoming, outgoing);
}

////////////////////////////////////////////////////////////////////////////////

GlassMaterial::GlassMaterial(scalar refr_inside, scalar refr_outside) : 
  _nr(refr_outside/refr_inside)
{
  
}

spectrum GlassMaterial::reflectance(const IntersectionView&,
                                    const Vec3& UNUSED(incoming), const Vec3& UNUSED(outgoing)) const
{
  return spectrum::zero;
}

MaterialSample GlassMaterial::sample_bsdf(
  const IntersectionView&, const Vec3& incoming, Sampler& sampler) const
{
  auto normal = Vec3::z_axis;
  scalar az = std::abs(incoming.z);
  if (az < 0.0001)
  {
    return MaterialSample::invalid;
  }

  scalar nr = _nr;
  if (incoming.z < 0)
  {
    nr = 1 / _nr;
    normal *= -1;
  }
  scalar fr = fresnel_reflectance(incoming, normal, nr);
  scalar refl_prob = fr;
  scalar samp = sampler.sample_1d();

  if (samp < refl_prob)
  {
    // reflect
    return MaterialSample{incoming.reflect_over(normal), refl_prob, spectrum{fr / az}};
  }
  else
  {
    // transmit
    auto wo = refraction_direction(incoming, normal, nr);
    return MaterialSample{wo, 1 - refl_prob, spectrum{(1 - fr) / az}};
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

MaterialSample EmissiveMaterial::sample_bsdf(
  const IntersectionView&, const Vec3& incoming, Sampler& sampler) const
{
  return MaterialSample::invalid;
}

scalar EmissiveMaterial::pdf(const Vec3&, const Vec3&) const
{
  return 0.0;
}

spectrum EmissiveMaterial::emission(const IntersectionView& isect) const
{
  return texture->at_point(isect);
}
  
