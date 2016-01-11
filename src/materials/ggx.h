#pragma once

#include "vec3.h"
#include "sampler.h"

struct MNSample
{
  Vec3 m;;
  scalar p;
};

class MicrofacetDistribution
{
public:
  virtual MNSample sample_micronormal(const Vec3& incoming, Sampler& sampler) const = 0;
};

class GGX : public MicrofacetDistribution
{
public:
  GGX(scalar roughness);

  scalar g2(const Vec3& i, const Vec3& o, const Vec3& h) const;

  scalar pdf_micronormal(const Vec3& incoming, const Vec3& micronormal) const;
  MNSample sample_micronormal(const Vec3& incoming, Sampler& sampler) const override;

  scalar density(const Vec3& h) const;

  scalar g1(const Vec3& v, const Vec3& h) const;

  scalar g1_11(scalar tan_v) const;

  // visible normal sampling
  scalar sample_marginal_x_slope(scalar angle, scalar u) const;
  scalar sample_conditional_y_slope(scalar x_slope, scalar u) const;

  // pdfs
  scalar pdf_marginal_x_slope(const Vec3& scaled_incoming, const Vec3& scaled_m) const;
  scalar pdf_conditional_y_slope(const Vec3& scaled_m) const;

  scalar cdf_marginal_x_slope(const Vec3& s_incoming, scalar x_slope_theta) const;
  scalar cdf_conditional_y_slope(scalar y_slope, scalar x_slope) const;

  const scalar _r, _r2;
};
