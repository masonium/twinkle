#include "materials/experimental.h"
#include "math_util.h"

SimpleSmoothPlastic::SimpleSmoothPlastic(const Texture* tex, scalar roughness)
  : _base(roughness, tex)
{
}

spectrum SimpleSmoothPlastic::reflectance(
  const IntersectionView& view, const Vec3& incoming, const Vec3& outgoing) const
{
  return _base.reflectance(view, incoming, outgoing);
}

MaterialSample SimpleSmoothPlastic::sample_bsdf(
  const IntersectionView& isect, const Vec3& incoming, Sampler& sampler) const
{
  // choose between specular and diffuse based on the fresnel coefficient
  const scalar nr = 1 / 1.5;

  scalar fr_r = fresnel_reflectance(incoming, Vec3::z_axis, nr);
  Vec3 outgoing;
  if (sampler.sample_1d() < fr_r)
  {
    auto outgoing = incoming.reflect_over(Vec3::z_axis);

    // specular + underlying diffuse (from remaining transmitted specular)
    spectrum refl = spectrum{fr_r} + (1 - fr_r) * _base.reflectance(isect, incoming, outgoing);

    return MaterialSample{outgoing, fr_r, refl};
  }
  else
  {
    // diffuse direction
    auto diffuse_sample = _base.sample_bsdf(isect, incoming, sampler);

    scalar p = (1 - fr_r) * diffuse_sample.prob;

    // only the underlying diffuse
    auto reflectance = (1 - fr_r) * diffuse_sample.reflectance;

    return MaterialSample{diffuse_sample.direction, p, reflectance};
  }
}

scalar SimpleSmoothPlastic::pdf(const Vec3& incoming, const Vec3& outgoing) const
{
  // assume it came from the diffuse component
  return _base.pdf(incoming, outgoing);
}

std::string SimpleSmoothPlastic::to_string() const
{
  using namespace std::literals::string_literals;
  return "PSS{"s + _base.to_string() + "}";
}
