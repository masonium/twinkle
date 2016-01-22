#include "rough_glass_material.h"
#include <vector>

using std::make_shared;
using std::cerr;
using std::endl;

RoughGlassBSDF::RoughGlassBSDF(scalar r, scalar ref_inside, scalar ref_outside)
  : _ggx{r}, _roughness(r), _nr(ref_outside/ref_inside)
{
}

scalar RoughGlassBSDF::pdf(const Vec3& UNUSED(incoming), const Vec3& UNUSED(outgoing)) const
{
  assert("unimplemented.");
  return 0.0;
}

Vec3 m_hr(const Vec3& i, const Vec3& o)
{
  return (i + o).normal() * sign(i.z);
}

Vec3 m_ht(const Vec3& i, const Vec3& o, scalar nr)
{
  return -(i * nr + o).normal();
}
scalar RoughGlassBSDF::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  if (incoming.z < 0)
    return reflectance(-incoming, -outgoing, 1/_nr);
  else
    return reflectance(incoming, outgoing, _nr);
}

scalar RoughGlassBSDF::reflectance(const Vec3& incoming, const Vec3& outgoing, scalar nr) const
{
  const auto ainon = std::abs(incoming.z * outgoing.z);

  // reflection
  if (incoming.z * outgoing.z > 0)
  {
    const auto hr = m_hr(incoming, outgoing);

    const auto fresnel_refl = fresnel_reflectance(incoming, hr, nr);

    return fresnel_refl * _ggx.g2(incoming, outgoing, hr) * _ggx.density(hr) /
      (4 * ainon);
  }
  // refraction
  else
  {
    const auto ht = m_ht(incoming, outgoing, nr);
    
    const auto fresnel_refr = fresnel_transmittance(incoming, ht, nr);
    
    const auto i_ht = incoming.dot(ht), o_ht = outgoing.dot(ht);
    const auto angle_coefficient = std::abs(i_ht * o_ht) / ainon;

    const auto numerator =  fresnel_refr * _ggx.g2(incoming, outgoing, ht) * _ggx.density(ht);
    auto denom_sqrt = (nr * i_ht + o_ht);

    return angle_coefficient * numerator / (square(denom_sqrt) * square(nr));
  }
}

BSDFSample RoughGlassBSDF::sample(const Vec3& incoming, Sampler& sampler) const
{
  Vec3 wi = incoming;
  scalar nr = _nr;

  if (incoming.z < 0)
  {
    wi *= -1;
    nr = 1 / nr;
  }
  // Generate a microsurface normal.
  MNSample mnp = _ggx.sample_micronormal(wi, sampler);

  if (mnp.p == 0)
    return BSDFSample::invalid;

  assert(!mnp.m.isnan());

  // Reflect or refract, based on the micronormal
  scalar refl_prob = fresnel_reflectance_schlick(wi, mnp.m, nr);
  //scalar refl_prob = fresnel_reflectance_schlick(wi, Vec3::z_axis, nr);

  scalar jac = 0;
  scalar p;
  Vec3 wo;
    
  if (sampler.sample_1d() < refl_prob)
  {
    wo = wi.reflect_over(mnp.m);
    if (wo.z < 0)
      return BSDFSample::invalid;
    Vec3 half = m_hr(wi, wo);
    jac = 0.25 / std::abs(wo.dot(half));
    p = refl_prob;
  }
  else
  {
    wo = refraction_direction(wi, mnp.m, nr);
    if (wo.norm2() == 0)
      return BSDFSample::invalid;
    Vec3 half = m_ht(wi, wo, nr);
    jac = std::abs(wo.dot(half)) 
      / square(nr * wi.dot(half) + wo.dot(half));
    p = 1 - refl_prob;
  }

  // The actual probability of the outgoing vector is a modification of the
  // micro_normal probability.
  p *= mnp.p;// * jac;

  assert(!isnan(p));

  if (p < BSDF_PDF_EPSILON)
    return BSDFSample::invalid;

  scalar refl = this->reflectance(wi, wo, nr);
  if (incoming.z < 0)
    wo *= -1;
  return BSDFSample{wo, p, refl}; 
}

////////////////////////////////////////////////////////////////////////////////



RoughGlassMaterial::RoughGlassMaterial(scalar r, scalar ref_inside, scalar ref_outside)
  : bsdf(r, ref_inside, ref_outside)
{
}

spectrum RoughGlassMaterial::reflectance(const IntersectionView&,
                                         const Vec3& incoming, const Vec3& outgoing) const
{
  return spectrum{bsdf.reflectance(incoming, outgoing)};
}

MaterialSample RoughGlassMaterial::sample_bsdf(const IntersectionView&, 
                                     const Vec3& incoming, Sampler& sampler)  const
{
  return MaterialSample{bsdf.sample(incoming, sampler), spectrum{1.0}};
}

shared_ptr<Material> make_glass_material(scalar roughness,
                                         scalar n_inside, scalar n_outside)
{
  return make_shared<RoughGlassMaterial>(roughness, n_inside, n_outside);
}
