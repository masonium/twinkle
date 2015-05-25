#pragma once

#include "math_util.h"
#include "spectrum.h"

class BRDF
{
public:
  virtual scalar reflectance(const Vec3& incoming, const Vec3& outgoing,
                             const Vec3& normal) const = 0;

  virtual Vec3 sample(const Vec3& incoming, const Vec3& normal,
                      scalar r1, scalar r2, scalar& p) const = 0;
  virtual bool is_emissive() const
  {
    return false;
  }
  virtual spectrum emission() const
  {
    return spectrum::zero;
  }
};

class Diffuse : public BRDF
{
public:
  explicit Diffuse( scalar r_ ) : r(r_ / PI)
  {
  }

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing,
                     const Vec3& normal) const override
  {
    if (incoming.dot(normal) < 0)
      return 0.0f;
    return r;
  }

  Vec3 sample(const Vec3& incoming, const Vec3& normal,
              scalar r1, scalar r2, scalar& p) const override;

  scalar r;
};

class EmissiveBRDF : public BRDF
{
public:
  explicit EmissiveBRDF( const spectrum em_ ) : em(em_)
  {
  }

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing,
                     const Vec3& normal) const override
  {
    return 0.0;
  }

  Vec3 sample(const Vec3& incoming, const Vec3& normal,
              scalar r1, scalar r2, scalar& p) const override
  {
    p = 0;
    return Vec3::zero;
  }

  bool is_emissive() const override
  {
    return true;
  }
  spectrum emission() const
  {
    return em;
  }

private:
  spectrum em;
};
