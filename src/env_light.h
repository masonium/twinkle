#pragma once

#include <memory>
#include "spectrum.h"
#include "vec3.h"
#include "texture.h"
#include "light.h"

using std::shared_ptr;

class EnvironmentLight : public Light
{
public:
  EnvironmentLight(const Texture2D* tex);

  spectrum emission(const Vec3& dir) const;
  LightSample sample_emission(const Intersection&, Sampler&) const override;
  EmissionSample sample_emission(const Scene& scene, Sampler&) const override;

  spectrum emission(const Scene& scene, const IntersectionView& isect,
                    const EmissionSample& sample) const override;

  const Texture2D* texture;
};
