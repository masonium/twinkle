#include "materials/ggx.h"
#include "vec2.h"

scalar GGX::density(const Vec3& m) const
{
  const auto cos_tm = m.z; // cosine of angle between micronormal and normal

  if (cos_tm <= 0.0001)
    return 0;

  const auto cos2_tm = square(cos_tm);
  const auto tan2_tm = 1.0 / cos2_tm - 1.0;

  return _r2 / (PI * square(cos2_tm * (_r2 + tan2_tm)));
}

scalar GGX::g1(const Vec3& v, const Vec3& h) const
{
  // The two directions have to be on the same side.
  if (v.dot(h) * v.z < 0)
    return 0;

  const auto cos_tv = v.z;
  const auto tan2_tv = 1.0 / square(cos_tv) - 1.0;

  return 2.0 / (1.0 + sqrt(1.0 + _r2 * tan2_tv));
}

scalar GGX::sample_marginal_x_slope(scalar theta, Sampler& sampler) const
{
  const scalar tan_vi = tan(theta);

  // G1 given visibility an unity roughness.
  const scalar G1 = 1.0 / (1.0 + sqrt(1.0 + square(tan_vi)));

  const scalar A = 2 * sampler.sample_1d() / G1 - 1.0;
  const scalar B = tan_vi;

  const auto A2 = square(A);
  const auto B2 = square(B);

  const auto c = B / (A2 - 1);
  const auto d = sqrt(B2 / square(A2 - 1) - (A2 - B2) / (A2 - 1));

  scalar x_m2 = c + d;

  if (A < 0 || x_m2 > 1/tan_vi)
    return c - d;
  else
    return x_m2;
}

scalar ggx_cy_inverse_func(scalar u)
{
  const auto numer = u * (0.46341 + u * (-0.73369 + u * 0.27385));
  const auto denom = 0.597999 + u * (-1 + u * (0.309420 + u * 0.093073));
  return numer / denom;
}

scalar GGX::sample_conditional_y_slope(scalar x_slope, Sampler& sampler) const
{
  scalar u = sampler.sample_1d();
  scalar s = 1.0;
  if (u <= 0.5)
  {
    u = 2 * (u - 0.5);
  }
  else
  {
    s = -1;
    s = 2 * (0.5 - u);
  }

  return s * ggx_cy_inverse_func(u) * sqrt(1 + square(x_slope));
}

Vec3 GGX::sample_micronormal(const Vec3& incoming, Sampler& sampler) const
{
  const auto scale_incoming = Vec3{-incoming.x / _r, -incoming.y / _r, incoming.z}.normal();
  scalar up_angle, tangent_plane_angle;
  scale_incoming.to_euler(up_angle, tangent_plane_angle);

  Vec2 tslopes;
  tslopes[0] = sample_marginal_x_slope(up_angle, sampler);
  tslopes[1] = sample_conditional_y_slope(tslopes[0], sampler);

  Vec2 slopes = _r * tslopes.rotate(tangent_plane_angle);

  auto m = Vec3(-slopes[0], -slopes[1], 1.0).normal();
  return m;
}
