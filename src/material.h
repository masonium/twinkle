#pragma once

#include <memory>
#include "base.h"
#include "spectrum.h"
#include "bsdf.h"
#include "intersection.h"
#include "sampler.h"
#include "texture.h"

using std::shared_ptr;
using std::unique_ptr;

class Material : public Base
{
public:
  virtual spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const = 0;
  
  virtual Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                           scalar& p, spectrum& reflectance) const = 0;

  virtual scalar pdf(const Vec3& incoming, const Vec3& outgoing) const = 0;

  virtual bool is_emissive(const IntersectionView&) const
  {
    return false;
  }
  
  virtual spectrum emission(const IntersectionView&) const
  {
    return spectrum::zero;
  }

  virtual std::string to_string() const override { return "Material"; }

  virtual ~Material() {}
};

class RoughMaterial : public Material
{
public:
  RoughMaterial(scalar roughness, Texture* tex);

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;
  
  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                   scalar& p, spectrum& reflectance) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

private:
  unique_ptr<BRDF> brdf;
  Texture* texture;
};

class SolidColor;
class RoughColorMaterial : public RoughMaterial
{
public:
  RoughColorMaterial(scalar roughness, const spectrum& color);

  std::string to_string() const override;

private:
  spectrum c;
  unique_ptr<SolidColor> sc;
};

class MirrorMaterial : public Material
{
public:
  MirrorMaterial()
  {
  }

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;
  
  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                   scalar& p, spectrum& reflectance) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

  virtual string to_string() const override { return "Mirror"; }
private:
  PerfectMirrorBRDF brdf;
};

class GlassMaterial : public Material
{
public:
  GlassMaterial(scalar ref_inside = refraction_index::CROWN_GLASS,
                scalar ref_outside = refraction_index::AIR);

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;

  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                   scalar& p, spectrum& reflectance) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;
private:
  scalar nr_inside, nr_outside;
};


class EmissiveMaterial : public Material
{
public:
  EmissiveMaterial(const Texture*);

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;

  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                   scalar& p, spectrum& reflectance) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

  bool is_emissive(const IntersectionView& isect) const override
  {
    return true;
  }
  spectrum emission(const IntersectionView& isect) const override;
  
private:
  const Texture* texture;
};
