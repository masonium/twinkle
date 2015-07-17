#pragma once

#include "math_util.h"
#include "spectrum.h"
#include "vec3.h"
#include "sampler.h"

class BRDF
{
public:
  virtual scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const = 0;

  virtual Vec3 sample(const Vec3& incoming, Sampler& sampler,
                      scalar& p, scalar& reflectance) const = 0;
  virtual bool is_emissive() const
  {
    return false;
  }
  virtual scalar emission() const
  {
    return 0;
  }

  virtual ~BRDF() { }
};

class Lambertian : public BRDF
{
public:
  explicit Lambertian( scalar r_) : r(r_ / PI)
  {
  }

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const override;

  Vec3 sample(const Vec3& incoming, Sampler& s,
              scalar& p, scalar& reflectance) const override;

  scalar r;
};

class OrenNayar : public BRDF
{
public:
  OrenNayar(scalar refl, scalar roughness);
  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const override;
  
  Vec3 sample(const Vec3& incoming, Sampler& s,
              scalar& p, scalar& reflectance) const override;
  
private:
  const scalar A, B;
  const scalar rpi;
};

class PerfectMirrorBRDF : public BRDF
{
public:
  explicit PerfectMirrorBRDF( )
  {
  }

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const override
  {
    return 0.0;
  }

  Vec3 sample(const Vec3& incoming, Sampler& s,
              scalar& p, scalar& reflectance) const override
  {
    p = 1.0;
    
    reflectance = 1.0 / incoming.z;
    
    return incoming.reflect_over(Vec3::z_axis);
  }
};

namespace refraction_index
{
  const scalar CROWN_GLASS = 1.52;
  const scalar PYREX = 1.47;
  const scalar WATER = 1.333;
  const scalar ICE = 1.31;
  const scalar AIR = 1.0003;
  const scalar VACUUM = 1.0;
}
