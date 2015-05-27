#pragma once

#include "math_util.h"
#include "spectrum.h"
#include "vec3.h"

class BRDF
{
public:
  virtual scalar reflectance(const Vec3& incoming, const Vec3& outgoing,
                             const Vec3& normal) const = 0;

  virtual Vec3 sample(const Vec3& incoming, const Vec3& normal,
                      scalar r1, scalar r2, scalar& p, scalar& reflectance) const = 0;
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
  explicit Diffuse( scalar r_ ) : r(r_ / (2.0 * PI))
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
              scalar r1, scalar r2, scalar& p, scalar& reflectance) const override;

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
              scalar r1, scalar r2, scalar& p, scalar& reflectance) const override
  {
    p = 0;
    reflectance = 0;
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


class PerfectMirrorBRDF : public BRDF
{
public:
  explicit PerfectMirrorBRDF( )
  {
  }

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing,
                     const Vec3& normal) const override
  {
    return 0.0;
  }

  Vec3 sample(const Vec3& incoming, const Vec3& normal,
              scalar r1, scalar r2, scalar& p, scalar& reflectance) const override
  {
    p = 1.0;
    reflectance = 1.0;
    Vec3 proj = incoming.projectOnto(normal);
    return proj * 2.0 - incoming;
  }
};
