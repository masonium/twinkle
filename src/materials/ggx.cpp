#include <cassert>
#include "materials/ggx.h"
#include "vec2.h"
#include "util.h"

GGX::GGX(scalar roughness) : _r(roughness), _r2(_r*_r)
{
}

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

scalar GGX::g2(const Vec3& i, const Vec3& o, const Vec3& h) const
{
  return g1(i, h) * g1(o, h);
}

scalar GGX::g1_11(scalar tan_vi) const
{
  return 2.0 / (1.0 + sqrt(1.0 + tan_vi * tan_vi));
}

scalar GGX::sample_marginal_x_slope(scalar theta, scalar u1) const
{
  const scalar tan_vi = tan(theta);

  // G1 given visibility an unity roughness.
  const scalar G1 = g1_11(tan_vi);

  const scalar A = 2 * u1 / G1 - 1.0;
  const scalar B = tan_vi;

  const auto A2 = square(A);
  const auto B2 = square(B);

  const auto dA21 = 1.0 / (A2 - 1);
  const auto c = B * dA21;
  const auto d = sqrt((B2 * dA21 - (A2 - B2)) * dA21);

  scalar x_m2 = c + d;

  if (A < 0 || x_m2 > 1/tan_vi)
    return c - d;
  else
    return x_m2;
}

scalar GGX::pdf_marginal_x_slope(const Vec3& scaled_incoming, const Vec3& scaled_m) const
{
  scalar cos_i = scaled_incoming.z;
  scalar sin_i = sqrt(1 - square(cos_i));
  scalar x_slope = - scaled_m.x / scaled_m.z;
  scalar dp = cos_i - x_slope * sin_i;
  if (dp <= 0)
    return 0;
  scalar G1 = g1_11(sin_i/cos_i);
  return G1 / cos_i * dp / (2.0 * pow(square(x_slope) + 1, 1.5));
}

scalar GGX::cdf_marginal_x_slope(const Vec3& s_incoming, scalar x_slope_theta) const
{
  scalar ci = s_incoming.z;
  scalar si = sqrt(1 - ci*ci);

  scalar G1 = g1_11(si/ci);

  if (x_slope_theta < -PI/2 + 0.0001)
    return 0;

  if (x_slope_theta > PI/2 - 0.0001)
    return 1.0;

  scalar x_slope = tan(x_slope_theta);

  if (x_slope > ci/si)
    return 1.0;

  scalar val = 0.5 * G1 / ci * ((si + ci * x_slope) / sqrt(1 + square(x_slope)) + ci);
  return clamp(val, 0.0, 1.0);
}

scalar ggx_cy_inverse_func(double u)
{
  return (u*(u*(u*0.27385-0.73369)+0.46341)) / (u*(u*(u*0.093073+0.309420)-1.000000)+0.597999);
}

scalar GGX::sample_conditional_y_slope(scalar x_slope, scalar u) const
{
  scalar s;
  if (u >= 0.5)
  {
    s = 1.0;
    u = 2 * (u - 0.5);
  }
  else
  {
    s = -1.0;
    u = 2 * (0.5 - u);
  }
  return s * ggx_cy_inverse_func(u) * sqrt(1 + square(x_slope));
}

scalar GGX::pdf_conditional_y_slope(const Vec3& scaled_m) const
{
  scalar x_slope = - scaled_m.x / scaled_m.z;
  scalar y_slope = - scaled_m.y / scaled_m.z;
  scalar num =  1 / (M_PI * square(1 + square(x_slope) + square(y_slope)));
  scalar den = 1 / (2.0 * pow(square(x_slope) + 1, 1.5));

  return num / den;
}

scalar GGX::cdf_conditional_y_slope(scalar y_slope, scalar x_slope) const
{
  scalar y_slope_theta = atan(y_slope);
  if (y_slope_theta < -PI/2 + 0.0001)
    return 0;

  if (y_slope_theta > PI/2 - 0.0001)
    return 1.0;

  scalar y = y_slope;
  scalar x = x_slope;
  const scalar a2 = 1 + x*x;
  const scalar a = sqrt(a2);

  return 0.5 + (a * y / (a2 + y*y) + atan(y_slope /a)) / M_PI;
}

scalar GGX::pdf_micronormal_slopes(const Vec3& m) const
{
  scalar mx = m.x / (_r * m.z);
  scalar my = m.y / (_r * m.z);
  return 1.0 / (M_PI * _r2 * square(mx*mx + my*my + 1));
}

scalar GGX::pdf_micronormal(const Vec3& incoming, const Vec3& m) const
{
  return std::abs(g1(incoming, m) * m.dot(incoming) * pdf_micronormal_slopes(m) / incoming.z);
}

MNSample GGX::sample_micronormal(const Vec3& incoming, Sampler& sampler) const
{
  if (incoming.z < 0.001)
    return MNSample{Vec3::zero, 0};

  const auto scale_incoming = Vec3{incoming.x * _r, incoming.y * _r, incoming.z}.normal();
  scalar up_angle, tangent_plane_angle;
  scale_incoming.to_euler(tangent_plane_angle, up_angle);

  Vec2 tslopes;
  auto u = sampler.sample_2d();
  tslopes[0] = sample_marginal_x_slope(up_angle, u[0]);
  tslopes[1] = sample_conditional_y_slope(tslopes[0], u[1]);

  Vec2 slopes = _r * tslopes.rotate(tangent_plane_angle);

  auto m = Vec3(-slopes[0], -slopes[1], 1.0).normal();

  scalar p = pdf_micronormal(incoming, m);

  if (p < BSDF_PDF_EPSILON)
    return MNSample{Vec3::zero, 0};

  return MNSample{m, p};
}
