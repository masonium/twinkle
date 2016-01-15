#pragma once

#include "material.h"
#include "ggx.h"

/**
 * This file implements a material for rough refractive surfaces, based on the
 * paper "Microfacet Models for Refraction through Rough Surfaces" by Walter
 * et. al.
 */
class RoughGlassBSDF
{
public:
  RoughGlassBSDF(scalar roughness,
                 scalar ref_inside = refraction_index::CROWN_GLASS,
                 scalar ref_outside = refraction_index::AIR);

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const;

  Vec3 sample(const Vec3& incoming, Sampler& sampler,
              scalar& p, scalar& reflectance) const;

private:
  GGX ggx;
  scalar roughness;
  scalar refr_incoming, refr_outgoing;
};

class RoughGlassMaterial : public Material
{
public:
  RoughGlassMaterial(scalar roughness,
                     scalar ref_inside = refraction_index::CROWN_GLASS,
                     scalar ref_outside = refraction_index::AIR);

  spectrum reflectance(const IntersectionView&,
                       const Vec3& incoming, const Vec3& outgoing) const override;

  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                   scalar& p, spectrum& reflectance) const override;

  scalar pdf(const Vec3&, const Vec3&) const override { return 0.0; };

private:
  RoughGlassBSDF bsdf;
};

shared_ptr<Material> make_glass_material(scalar roughness,
                                         scalar n_inside = refraction_index::CROWN_GLASS,
                                         scalar n_outside = refraction_index::AIR);
