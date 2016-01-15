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
  scalar bsdf_reflection_contribution = 0;
  scalar bsdf_refraction_contribution = 0;
  const auto ainon = std::abs(incoming.z * outgoing.z);

  scalar nr = _nr;
  if (incoming.z < 0)
    nr = 1 / nr;

  {
    const auto hr = m_hr(incoming, outgoing);

    const auto fresnel_refl = fresnel_reflectance(incoming, hr, nr);

    bsdf_reflection_contribution = fresnel_refl * _ggx.g2(incoming, outgoing, hr) * _ggx.density(hr) /
        (4 * ainon);
  }

  {
    const auto ht = m_ht(incoming, outgoing, nr);
    
    const auto fresnel_refr = fresnel_transmittance(incoming, ht, nr);
    
    const auto i_ht = incoming.dot(ht), o_ht = outgoing.dot(ht);
    const auto angle_coefficient = std::abs(i_ht * o_ht) / ainon;

    const auto numerator =  fresnel_refr * _ggx.g2(incoming, outgoing, ht) * _ggx.density(ht);
    auto denom_sqrt = (nr * i_ht + o_ht);

    bsdf_refraction_contribution = angle_coefficient * numerator / square(denom_sqrt);
  }

  return bsdf_reflection_contribution + bsdf_refraction_contribution;
}

BSDFSample RoughGlassBSDF::sample(const Vec3& incoming, Sampler& sampler) const
{
  // Generate a microsurface normal.
  MNSample mnp = _ggx.sample_micronormal(incoming, sampler);

  // Reflect or refract, based on the micronormal
  //scalar refl_prob = fresnel_reflectance_schlick(incoming, mnp.m, refr_incoming, refr_outgoing);
  scalar refl_prob = 0;

  scalar jac = 0;
  scalar p;
  Vec3 outgoing;
  scalar nr = incoming.z < 0 ? 1 / _nr : _nr;
    
  if (sampler.sample_1d() < refl_prob)
  {
    outgoing = incoming.reflect_over(mnp.m);
    Vec3 half = m_hr(incoming, outgoing);
    jac = 0.25 / std::abs(outgoing.dot(half));
    p = refl_prob;
  }
  else
  {
    outgoing = refraction_direction(incoming, mnp.m, nr);
    Vec3 half = m_ht(incoming, outgoing, nr);
    jac = std::abs(outgoing.dot(half)) 
      / square(nr * incoming.dot(half) + outgoing.dot(half));
    p = 1 - refl_prob;
  }

  // The actual probability of the outgoing vector is a modification of the
  // micro_normal probability.
  p *= mnp.p * jac;

  scalar refl = this->reflectance(incoming, outgoing.normal());
  return BSDFSample{outgoing, p, refl}; 
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
