#pragma once

#include "material.h"

/**
 * This file implements a material for rough refractive surfaces, based on the
 * paper "Microfacet Models for Refraction through Rough Surfaces" by Walter
 * et. al.
 */

class GGXDistribution
{
public:
  GGXDistribution(scalar width);

  scalar density(const Vec3& micro_normal) const;

  scalar smith_shadow(const Vec3& incoming, const Vec3& outgoing,
                      const Vec3& micro_normal) const;

private:
  scalar g1(scalar cos_angle) const;

  scalar width, width_sq;
};

template <typename Distribution>
class RoughGlassMaterial : public Material
{
public:
  RoughGlassMaterial(scalar roughness,
                     scalar ref_inside = refraction_index::CROWN_GLASS,
                     scalar ref_outside = refraction_index::AIR);

  scalar reflectance(const Vec3& incoming, const Vec3& outgoing) const override;

  Vec3 sample_bsdf(const Vec3& incoming, Sampler& sampler,
                   scalar& p, scalar& reflectance) const override;

  spectrum texture_at_point(const Intersection& isect) const override;

private:
  Distribution dt;
  scalar roughness;
  scalar refr_incoming, refr_outgoing;
};

enum RoughGlassDistribution : uint8_t
{
  GGX = 0
};

shared_ptr<Material> make_glass_material(RoughGlassDistribution, scalar);
