#include "rough_mirror.h"

RoughMirror::RoughMirror(scalar r) : _ggx(r)
{
}

MaterialSample RoughMirror::sample_bsdf(
  const IntersectionView&, const Vec3& incoming, Sampler& sampler) const
{
  // pick a micronormal
  auto mnp = _ggx.sample_micronormal(incoming, sampler);

  if (mnp.p <= 0)
    return MaterialSample::invalid;

  // reflect along that micronormal
  MaterialSample ms;
  ms.direction = incoming.reflect_over(mnp.m);

  // compute the overall p
  scalar dot_oh = (incoming.dot(mnp.m));
  ms.prob = mnp.p * 0.25 / dot_oh;

  if (ms.direction.z <= 0.0001)
    ms.reflectance = spectrum::zero;
  else
  {
    ms.reflectance = spectrum{1.0} * _ggx.density(mnp.m) * _ggx.g2(incoming, ms.direction, mnp.m);
    ms.reflectance /= (4.0 * incoming.z * ms.direction.z);
  }
  return ms;
}

spectrum RoughMirror::reflectance(const IntersectionView&,
                                  const Vec3& incoming, const Vec3& outgoing) const
{
  return spectrum{0.0};
}

scalar RoughMirror::pdf(const Vec3& incoming, const Vec3& outgoing) const
{
  return 0;
}
