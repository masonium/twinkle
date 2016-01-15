#pragma once

#include "material.h"

class SimpleSmoothPlastic : public Material
{
public:
  SimpleSmoothPlastic(const Texture*, scalar roughness);

  spectrum reflectance(const IntersectionView&, const Vec3& incoming, const Vec3& outgoing) const override;

  Vec3 sample_bsdf(const IntersectionView&, const Vec3& incoming, Sampler& sampler,
                           scalar& p, spectrum& reflectance) const override;

  scalar pdf(const Vec3& incoming, const Vec3& outgoing) const override;

  std::string to_string() const override;

private:
  RoughMaterial _base;
};
