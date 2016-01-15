#pragma once

#include "math_util.h"
#include "spectrum.h"
#include "vec3.h"
#include "sampler.h"

struct BSDFSample
{
  BSDFSample() {} 
  BSDFSample(const Vec3& d, scalar p, scalar refl)
    : direction(d), prob(p), reflectance(refl)
  {
  }

  static BSDFSample invalid;

  Vec3 direction;
  scalar prob;
  scalar reflectance;
};

class BRDF
{
public:
  virtual scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const = 0;

  virtual BSDFSample sample(const Vec3& incoming, Sampler& sampler) const = 0;
  
  virtual scalar pdf(const Vec3& incoming, const Vec3& outgoing) const = 0;

  virtual ~BRDF() { }
};

class Lambertian : public BRDF
{
public:
  explicit Lambertian(scalar r_) : r(r_ / PI)
  {
  }

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const override;

  BSDFSample sample(const Vec3& incoming, Sampler& s) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

  scalar r;
};

class OrenNayar : public BRDF
{
public:
  OrenNayar(scalar refl, scalar roughness);
  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const override;
  
  BSDFSample sample(const Vec3& incoming, Sampler& s) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

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
  BSDFSample sample(const Vec3& incoming, Sampler& s) const override;


  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override
  {
    return 0.0f;
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
