#pragma once

#include "material.h"
#include "ggx.h"
#include "bsdf.h"

class RoughMirror : public Material
{
public:
  RoughMirror(scalar roughness);

  spectrum reflectance(const IntersectionView&,
                       const Vec3& incoming, const Vec3& outgoing) const override;

  MaterialSample sample_bsdf(
    const IntersectionView&, const Vec3& incoming, Sampler& sampler) const override;

  scalar pdf(const Vec3&, const Vec3&) const override;

private:
  GGX _ggx;
};
