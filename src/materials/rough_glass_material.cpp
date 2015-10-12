#include "rough_glass_material.h"
#include <vector>

using std::make_shared;
using std::cerr;
using std::endl;

GGXDistribution::GGXDistribution(scalar w) : width(w), width_sq(w*w)
{
}

Vec3 GGXDistribution::sample_micro_normal(Sampler& s, scalar& p) const
{
  auto x = s.sample_2d();
  scalar theta = 2 * PI * x[0];
  scalar phi = atan( width * sqrt(x[1]) / sqrt(1 - x[1]) );
  Vec3 out = Vec3::from_euler(theta, phi);

  p = density(out) * out.z;
  return out;
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

////////////////////////////////////////////////////////////////////////////////

template <typename D>
RoughGlassBSDF<D>::RoughGlassBSDF(scalar r, scalar ref_inside, scalar ref_outside)
  : dt{r}, roughness(r), refr_incoming(ref_outside), refr_outgoing(ref_inside)
{
}

template <typename D>
scalar RoughGlassBSDF<D>::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  scalar bsdf_reflection_contribution = 0;
  scalar bsdf_refraction_contribution = 0;
  const auto ainon = std::abs(incoming.z * outgoing.z);

  {
    const auto hr = (incoming + outgoing).normal() * sign(incoming.dot(outgoing));

    const auto fresnel_refl = fresnel_reflectance(incoming, hr, refr_incoming, refr_outgoing);

    bsdf_reflection_contribution = fresnel_refl * dt.smith_shadow(incoming, outgoing, hr) * dt.density(hr) /
        (4 * ainon);
  }

  {
    const auto ht = (-incoming * refr_incoming - outgoing * refr_outgoing).normal();
    
    const auto fresnel_refr = fresnel_transmittance(incoming, ht, refr_incoming, refr_outgoing);
    
    const auto i_ht = incoming.dot(ht), o_ht = outgoing.dot(ht);
    const auto angle_coefficient = std::abs(i_ht * o_ht) / ainon;

    const auto numerator = refr_outgoing * refr_outgoing * fresnel_refr * dt.smith_shadow(incoming, outgoing, ht) * dt.density(ht);
    auto denom_sqrt = (refr_incoming * i_ht * refr_outgoing * o_ht);

    bsdf_refraction_contribution = angle_coefficient * numerator / square(denom_sqrt);
  }

  return bsdf_reflection_contribution + bsdf_refraction_contribution;
}

template <typename D>
Vec3 RoughGlassBSDF<D>::sample(const Vec3& incoming, Sampler& sampler,
                               scalar& p, scalar& refl) const
{
  scalar sampling_roughness = roughness * 4.0;
  D sample_dt(sampling_roughness);

  // Generate a microsurface normal.
  scalar mnp;
  Vec3 micro_normal = sample_dt.sample_micro_normal(sampler, mnp);
  if (mnp == 0)
  {
    p = 0;
    return Vec3::zero;
  }

  // Reflect or refract, based on the micronormal
  scalar refl_prob = fresnel_reflectance(incoming, micro_normal, refr_incoming, refr_outgoing);

  scalar jac = 0;
  Vec3 outgoing;
  if (sampler.sample_1d() < refl_prob)
  {
    outgoing = incoming.reflect_over(micro_normal);
    Vec3 half = (incoming + outgoing) * sign(incoming.dot(outgoing));
    jac = 0.25 / std::abs(outgoing.dot(half.normal()));
    // jac *= refl_prob;
  }
  else
  {
    outgoing = refraction_direction(incoming, micro_normal, refr_incoming, refr_outgoing);
    Vec3 half = -incoming * refr_incoming - outgoing * refr_outgoing;
    jac = refr_outgoing * refr_outgoing * std::abs(outgoing.dot(half.normal())) / half.norm2();
    // jac *= (1 - refl_prob);
  }

  // The actual probability of the outgoing vector is a modification of the
  // micro_normal probability.
  p = mnp * jac;

  if (mnp < 0.0001)
    cerr << mnp << " " << jac << " " << micro_normal.z << endl;

  refl = this->reflectance(incoming, outgoing);
  return outgoing;
}

////////////////////////////////////////////////////////////////////////////////



template <typename D>
RoughGlassMaterial<D>::RoughGlassMaterial(scalar r, scalar ref_inside, scalar ref_outside)
  : bsdf(r, ref_inside, ref_outside)
{
}

template <typename D>
spectrum RoughGlassMaterial<D>::reflectance(const IntersectionView&,
                                            const Vec3& incoming, const Vec3& outgoing) const
{
  return spectrum{bsdf.reflectance(incoming, outgoing)};
}

template <typename D>
Vec3 RoughGlassMaterial<D>::sample_bsdf(const IntersectionView&, 
                                        const Vec3& incoming, Sampler& sampler,
                                        scalar& p, spectrum& reflectance)  const
{
  scalar scalar_refl;
  Vec3 dir = bsdf.sample(incoming, sampler, p, scalar_refl);

  reflectance = spectrum{scalar_refl};
  return dir;
}

shared_ptr<Material> make_glass_material(RoughGlassDistribution dist, scalar roughness,
                                         scalar n_inside, scalar n_outside)
{
  if (roughness == 0)
    return make_shared<GlassMaterial>(n_inside, n_outside);

  return make_shared<RoughGlassMaterial<GGXDistribution>>(roughness, n_inside, n_outside);
}


////////////////////////////////////////////////////////////////////////////////

void ggx_test()
{
  using std::vector;
  using std::cout;
  using std::endl;

  auto mat = make_shared<RoughGlassMaterial<GGXDistribution>>(0.344, 1.51);

  scalar incidence_angle = PI/6.0;

  for (int i = 150; i < 210; ++i)
  {
    scalar rad = i * PI / 180.0;
    Vec3 out(sin(rad), 0, cos(rad));
    cout << i << " " << mat->bsdf.reflectance(Vec3(sin(incidence_angle), 0, cos(incidence_angle)), out) << endl;
  }
}
