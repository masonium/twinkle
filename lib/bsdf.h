#pragma once

#include "math_util.h"
#include "spectrum.h"

class BRDF
{
public:
  virtual scalar reflectance(Vec3 incoming, Vec3 outgoing, Vec3 normal) const = 0;
  virtual Vec3 sample(scalar r1, scalar r2) const = 0;
  virtual bool is_emissive() const
  {
    return false;
  }
  virtual spectrum emission() const
  {
    return spectrum_zero;
  }
};

class Diffuse : public BRDF
{
public:
  explicit Diffuse( scalar r_ ) : r(r_ / PI)
  {
  }
  
  scalar reflectance(Vec3 incoming, Vec3 outgoing, Vec3 normal) const override
  {
    if (incoming.dot(normal) < 0)
      return 0.0f;
    return r;
  }

  Vec3 sample(scalar r1, scalar r2) const override;

  
  scalar r;
};

class EmissiveBRDF : public BRDF
{
public:
  explicit EmissiveBRDF( const spectrum em_ ) : em(em_)
  {
  }
  
  scalar reflectance(Vec3 incoming, Vec3 outgoing, Vec3 normal) const override
  {
    return 0.0;
  }

  Vec3 sample(scalar r1, scalar r2) const override
  {
    return vec_zero; 
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

