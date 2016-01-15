#include "materials/experimental.h"
#include "math_util.h"

SimpleSmoothPlastic::SimpleSmoothPlastic(const Texture* tex, scalar roughness)
  : _base(roughness, tex)
{
}

spectrum SimpleSmoothPlastic::reflectance(const IntersectionView& view, const Vec3& incoming, const Vec3& outgoing) const
{
  return _base.reflectance(view, incoming, outgoing);
}

Vec3 SimpleSmoothPlastic::sample_bsdf(const IntersectionView& isect, const Vec3& incoming, Sampler& sampler,
                                      scalar& p, spectrum& reflectance) const
{
  // choose between specular and diffuse based on the fresnel coefficient
  const scalar n1 = refraction_index::AIR, n2 = 1.5;

  scalar fr_r = fresnel_reflectance(incoming, Vec3::z_axis, n1, n2);
  reflectance = spectrum{0};
  Vec3 outgoing;
  if (sampler.sample_1d() < fr_r)
  {
    // specular direction
    p = fr_r;

    outgoing = incoming.reflect_over(Vec3::z_axis);

    // specular + underlying diffuse (from remaining transmitted specular)
    reflectance = spectrum{fr_r} + (1 - fr_r) * _base.reflectance(isect, incoming, outgoing);
  }
  else
  {
    // diffuse direction
    scalar diffuse_p;
    spectrum diffuse_refl;
    outgoing = _base.sample_bsdf(isect, incoming, sampler, diffuse_p, diffuse_refl);

    p = (1 - fr_r) * diffuse_p;

    // only the underlying diffuse
    reflectance = (1 - fr_r) * diffuse_refl;
  }

  return outgoing;
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
