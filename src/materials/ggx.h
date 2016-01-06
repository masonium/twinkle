#pragma once

#include "vec3.h"
#include "sampler.h"

struct DistributionSample
{
  Vec3 reflection_dir;
  scalar p;
};

class MicrofacetDistribution
{
public:
  virtual DistributionSample sample_direction(const Vec3& incoming, Sampler& sampler) const = 0;

  virtual scalar reflectance(scalar schlick_f0, const Vec3& incoming, const Vec3& outgoing,
                             scalar& shadowing) const = 0;

  virtual scalar pdf(const Vec3& incoming, const Vec3& outgoing) const = 0;
};

class GGX : public MicrofacetDistribution
{
public:
  GGX(scalar roughness);

private:
  DistributionSample sample_direction(const Vec3& incoming, Sampler& sampler) const override;

  scalar reflectance(scalar schlick_f0, const Vec3& incoming,
                     const Vec3& outgoing, scalar& shadowing) const override;

  scalar pdf_micronormal(const Vec3& incoming, const Vec3& micronormal) const;

  Vec3 sample_micronormal(const Vec3& incoming, Sampler& sampler) const;

private:
  scalar density(const Vec3& h) const;

  scalar g1(const Vec3& v, const Vec3& h) const;

  // visible normal sampling
  scalar sample_marginal_x_slope(scalar cos_vi, Sampler& sample) const;
  scalar sample_conditional_y_slope(scalar x_slope, Sampler& sampler) const;


  const scalar _r, _r2;
};
