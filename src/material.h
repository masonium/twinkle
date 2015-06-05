#pragma once
#include <memory>
#include "spectrum.h"
#include "bsdf.h"
#include "intersection.h"
#include "sampler.h"
#include "texture.h"

using std::shared_ptr;
using std::unique_ptr;

class Material
{
public:
  virtual scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const = 0;
  
  virtual Vec3 sample_bsdf(const Vec3& incoming, const Sample2D& sample,
                           scalar& p, scalar& reflectance) const = 0;
  
  virtual spectrum texture_at_point(const Intersection& isect) const = 0;

  virtual bool is_emissive() const
  {
    return false;
  }
  
  virtual spectrum emission(const Intersection&) const
  {
    return spectrum::zero;
  }

  virtual ~Material() {}
};

class RoughMaterial : public Material
{
public:
  RoughMaterial(scalar roughness, shared_ptr<Texture> tex);

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const override;
  
  Vec3 sample_bsdf(const Vec3& incoming, const Sample2D& sample,
                   scalar& p, scalar& reflectance) const override;

  spectrum texture_at_point(const Intersection& isect) const override;

private:
  unique_ptr<BRDF> brdf;
  shared_ptr<Texture> texture;
};


class RoughColorMaterial : public RoughMaterial
{
public:
  RoughColorMaterial(scalar roughness, const spectrum& color);
};
