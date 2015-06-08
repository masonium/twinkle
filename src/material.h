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
  virtual scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const
  {
    return 0;
  };
  
  virtual Vec3 sample_bsdf(const Vec3& incoming, const Sample2D& sample,
                           scalar& p, scalar& reflectance) const
  {
    p = 0;
    reflectance = 0;
    return Vec3::zero;
  }
  
  virtual spectrum texture_at_point(const Intersection& isect) const
  {
    return spectrum::zero;
  }

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


class MirrorMaterial : public Material
{
public:
  MirrorMaterial() : brdf(unique_ptr<BRDF>(new PerfectMirrorBRDF))
  {
  }
  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const override;
  
  Vec3 sample_bsdf(const Vec3& incoming, const Sample2D& sample,
                   scalar& p, scalar& reflectance) const override;

  spectrum texture_at_point(const Intersection& isect) const override;

private:
  unique_ptr<BRDF> brdf;
};

class GlassMaterial : public Material
{
public:
  GlassMaterial( scalar refr = refraction_index::CROWN_GLASS );

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const override;

  Vec3 sample_bsdf(const Vec3& incoming, const Sample2D& sample,
                   scalar& p, scalar& reflectance) const override;

  spectrum texture_at_point(const Intersection& isect) const override;

private:
  scalar nr;
};

class EmissiveMaterial : public Material
{
public:
  EmissiveMaterial(spectrum e) : emit(e) {}

  bool is_emissive() const override { return true; }

  spectrum emission(const Intersection& isect) const  override
  {
    return emit;
  }
  
private:
  spectrum emit;
};
