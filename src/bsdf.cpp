#include "bsdf.h"
#include "util.h"
#include "math_util.h"
#include "sampler.h"

BSDFSample BSDFSample::invalid(Vec3::zero, 0, 0);

BSDFSample Lambertian::sample(const Vec3& UNUSED(incoming), Sampler& sampler) const
{
  auto sample = sampler.sample_2d();
  
  Vec3 wo = cosine_weighted_hemisphere_sample(sample);

  return BSDFSample(wo, wo.z / PI, r);
}

scalar Lambertian::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  if (incoming.z * outgoing.z < 0)
    return 0.0f;
  return r;
}

scalar Lambertian::pdf(const Vec3& incoming, const Vec3& outgoing) const
{
  if (incoming.z * outgoing.z < 0)
    return 0.0f;
  return fabs(outgoing.z) / PI;
}

OrenNayar::OrenNayar(scalar refl, scalar rough) :
  A(1 - 0.5 * rough * rough / (rough * rough + 0.57)), 
  B(0.45 * rough * rough / (rough * rough + 0.09)),
  rpi(refl / PI)
{
  
}

BSDFSample OrenNayar::sample(const Vec3& incoming, Sampler& sampler) const
{
  auto sample = sampler.sample_2d();
  Vec3 wo = cosine_weighted_hemisphere_sample(sample);

  return BSDFSample(wo, wo.z / PI, this->reflectance(incoming, wo));
}

scalar OrenNayar::pdf(const Vec3& incoming, const Vec3& outgoing) const
{
  if (incoming.z * outgoing.z < 0)
    return 0.0f;
  return fabs(outgoing.z) / PI;
}

scalar OrenNayar::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  if (incoming.z * outgoing.z < 0)
    return 0.0f;

  scalar t1, t2, p1, p2;
  incoming.to_euler(t1, p1);
  outgoing.to_euler(t2, p2);
  
  return rpi * (A + B * max<scalar>(0, cos(t1 - t2)) * sin(max(p1, p2)) * tan(min(p1, p2)));
}

BSDFSample PerfectMirrorBRDF::sample(const Vec3& incoming, Sampler& UNUSED(s)) const
{
  if (incoming.z <= 0)
    return BSDFSample::invalid;

  return BSDFSample{incoming.reflect_over(Vec3::z_axis), 1.0, 1.0/incoming.z};
}
