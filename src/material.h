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

/*
 * Sample the direction from a material, returning other required information.
 */
struct MaterialSample
{
  MaterialSample() { }
  MaterialSample(const Vec3&, scalar, const spectrum&);
  MaterialSample(const BSDFSample&, const spectrum&);

  static MaterialSample invalid;

  Vec3 direction;
  scalar prob;
  spectrum reflectance;
};

class Material : public Base
{
public:
  virtual spectrum reflectance(const IntersectionView&, const Vec3& incoming,
                               const Vec3& outgoing) const = 0;
  
  virtual MaterialSample sample_bsdf(const IntersectionView&, const Vec3& incoming, 
                                     Sampler& sampler) const = 0;

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
  RoughMaterial(scalar roughness, const Texture* tex);

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, 
                       const Vec3& outgoing) const override;
  
  MaterialSample sample_bsdf(const IntersectionView&, const Vec3& incoming, 
                             Sampler& sampler) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

private:
  unique_ptr<BRDF> brdf;
  const Texture* texture;
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
  
  MaterialSample sample_bsdf(const IntersectionView&, const Vec3& incoming, 
                             Sampler& sampler) const override;

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

  MaterialSample sample_bsdf(const IntersectionView&, const Vec3& incoming, 
                             Sampler& sampler) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;
private:
  scalar _nr;
};


class EmissiveMaterial : public Material
{
public:
  EmissiveMaterial(const Texture*);

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;

  MaterialSample sample_bsdf(const IntersectionView&, const Vec3& incoming, 
                             Sampler& sampler) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

  bool is_emissive(const IntersectionView& isect) const override
  {
    return true;
  }
  spectrum emission(const IntersectionView& isect) const override;
  
private:
  const Texture* texture;
};
