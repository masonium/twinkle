#include "rough_glass_material.h"

GGXDistribution::GGXDistribution(scalar w) : width(w), width_sq(w*w)
{
}

scalar GGXDistribution::density(const Vec3& micro_normal) const
{
  scalar cos_nm = micro_normal.z;
  if (cos_nm <= 0)
  {
    return 0;
  }

  auto ca2 = cos_nm * cos_nm;
  auto ca4 = ca2 * ca2;
  auto tana2 = 1 / ca2 - 1;

  scalar r = (width_sq + tana2);

  return width_sq / (PI * ca4 * r * r);
}

scalar GGXDistribution::smith_shadow(const Vec3& incoming, const Vec3& outgoing, const Vec3& micro_normal) const
{
  auto cos_im = incoming.dot(micro_normal), cos_in = incoming.z;
  if (cos_im * cos_in <= 0)
    return 0;

  auto cos_om = outgoing.dot(micro_normal), cos_on = outgoing.z;
  if (cos_om * cos_on <= 0)
    return 0;
  return g1(cos_im) * g1(cos_om);
}

scalar GGXDistribution::g1(scalar cos_angle) const
{
  auto tan_angle2 = 1 / (cos_angle * cos_angle) - 1;
  return  2 / (1 + sqrt(1 + width_sq * tan_angle2));
}

template <typename D>
RoughGlassMaterial<D>::RoughGlassMaterial(scalar r, scalar ref_inside, scalar ref_outside)
  : dt{r}, roughness(r), refr_incoming(ref_outside), refr_outgoing(ref_inside)
{
}

template <typename D>
scalar RoughGlassMaterial<D>::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  const auto hr = (incoming + outgoing).normal() * sign(incoming.dot(outgoing));
  const auto ht = (-incoming * refr_incoming - outgoing * refr_outgoing).normal();

  const auto fresnel_refl = fresnel_reflectance(incoming, hr, refr_incoming, refr_outgoing);

  const auto ainon = abs(incoming.z * outgoing.z);

  const auto refl_term = fresnel_refl * dt.smith_shadow(incoming, outgoing, hr) * dt.density(hr) /
    (4 * ainon);

  const auto fresnel_refr = fresnel_transmittance(incoming, ht, refr_incoming, refr_outgoing);
  const auto i_ht = incoming.dot(ht), o_ht = outgoing.dot(ht);
  const auto angle_coefficient = abs(i_ht * o_ht) / ainon;
  const auto numerator = refr_outgoing * refr_outgoing * fresnel_refr * dt.smith_shadow(incoming, outgoing, ht) * dt.density(ht);
  auto denom_sqrt = (refr_incoming * i_ht * refr_outgoing * o_ht);
  scalar refr_term = angle_coefficient * numerator / square(denom_sqrt);

  return refl_term + refr_term;
}

template <typename D>

Vec3 RoughGlassMaterial<D>::sample_bsdf(const Vec3& incoming, Sampler& sampler,
                                          scalar& p, scalar& reflectance) const
{
  D sample_dt{1.2 - 0.2 * sqrt(abs(incoming.z))};

  // Generate a microsurface normal, then compute the reflectance based on that
  // microsurface normal.

  return Vec3::zero;
}
