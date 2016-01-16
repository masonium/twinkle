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

// The GGX slope distribution is:
// p(x_m, y_m) = 1/pi * 1/(a_x * a_y) * 1 / ( (x_m/a_x)^2 + (y_m/a_y)^2 + 1)
//
// where x_m = - H_x / H_z, y_m = -H_y / H_z

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

  scalar pdf_micronormal_slopes(const Vec3& m) const;

  // pdfs
  scalar pdf_marginal_x_slope(const Vec3& scaled_incoming, const Vec3& scaled_m) const;
  scalar pdf_conditional_y_slope(const Vec3& scaled_m) const;

  scalar cdf_marginal_x_slope(const Vec3& s_incoming, scalar x_slope_theta) const;
  scalar cdf_conditional_y_slope(scalar y_slope, scalar x_slope) const;

  const scalar _r, _r2;
};
