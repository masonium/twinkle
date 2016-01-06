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
  virtual Vec3 sample_micronormal(const Vec3& incoming, Sampler& sampler) const = 0;
};

class GGX : public MicrofacetDistribution
{
public:
  GGX(scalar roughness);

  DistributionSample sample_direction(const Vec3& incoming, Sampler& sampler) const;

  scalar reflectance(scalar schlick_f0, const Vec3& incoming,
                     const Vec3& outgoing, scalar& shadowing) const;

  scalar pdf_micronormal(const Vec3& incoming, const Vec3& micronormal) const;

  Vec3 sample_micronormal(const Vec3& incoming, Sampler& sampler) const override;

  scalar density(const Vec3& h) const;

  scalar g1(const Vec3& v, const Vec3& h) const;
  scalar g1_11(const Vec3& v) const;

  // visible normal sampling
  scalar sample_marginal_x_slope(scalar angle, scalar u) const;
  scalar sample_conditional_y_slope(scalar x_slope, scalar u) const;

  // pdfs
  scalar pdf_marginal_x_slope(const Vec3& scaled_incoming, const Vec3& scaled_m) const;
  scalar pdf_conditional_y_slope(const Vec3& scaled_incoming, const Vec3& scaled_m) const;

  scalar cdf_marginal_x_slope(const Vec3& s_incoming, scalar x_slope_theta);

  const scalar _r, _r2;
};
