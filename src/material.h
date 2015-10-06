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
  virtual spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const = 0;
  
  virtual Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                           scalar& p, spectrum& reflectance) const = 0;

  virtual bool is_emissive(const IntersectionView&) const
  {
    return false;
  }
  
  virtual spectrum emission(const IntersectionView&) const
  {
    return spectrum::zero;
  }

  virtual ~Material() {}
};

class RoughMaterial : public Material
{
public:
  RoughMaterial(scalar roughness, shared_ptr<Texture> tex);

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;
  
  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                   scalar& p, spectrum& reflectance) const override;

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
  MirrorMaterial()
  {
  }
  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;
  
  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                   scalar& p, spectrum& reflectance) const override;

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

private:
  scalar nr_inside, nr_outside;
};

/*
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
*/
