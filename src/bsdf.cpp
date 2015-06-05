#include "bsdf.h"
#include "math_util.h"
#include "sampler.h"

Vec3 Lambertian::sample(const Vec3& incoming, 
                        const Sample2D& sample, scalar& p,
                        scalar& reflectance) const
{
  Vec3 wo = cosine_weighted_hemisphere_sample(sample);

  p = wo.z / (2 * PI);
  reflectance = r;

  return wo;
}


OrenNayar::OrenNayar(scalar refl, scalar rough) :
  A(1 - 0.5 * rough * rough / (rough * rough + 0.33)), 
  B(0.45 * rough * rough / (rough * rough + 0.09)),
  rpi(refl / PI)
{
  
}

Vec3 OrenNayar::sample(const Vec3& incoming,
                       const Sample2D& sample, scalar& p,
                       scalar& reflectance) const
{
  Vec3 wo = cosine_weighted_hemisphere_sample(sample);

  p = wo.z / (2 * PI);
  reflectance = rpi;

  return wo;
}

scalar OrenNayar::reflectance(const Vec3& incoming, const Vec3& outgoing) const
{
  if (incoming.z < 0)
    return 0.0f;

  scalar t1, t2, p1, p2;
  incoming.to_euler(t1, p1);
  outgoing.to_euler(t2, p2);
  
  return rpi * (A + B * max<scalar>(0, cos(t1 - t2)) * sin(max(p1, p2)) * tan(min(p1, p2)));
}
