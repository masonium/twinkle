#include "math_util.h"
#include "vec3.h"
#include "vec2.h"
#include "mat33.h"
#include "ray.h"
#include <vector>
#include <cassert>
#include <algorithm>
#include <cmath>

using std::transform;
using std::vector;

scalar approx_gt(scalar x, scalar y)
{
  return x > y - SCALAR_EPSILON;
}

scalar qf(scalar a, scalar b, scalar c)
{
  const scalar disc = b * b - 4 * a * c;
  if (disc < 0)
    return -1;

  const scalar sd = sqrt(disc);
  const scalar small = -b - sd;
  if (small > 0)
    return small / (2.0 * a);
  return (-b + sd) / (2.0 * a);
}

Vec2 to_euler_uv(const Vec3& dir)
{
  scalar theta, phi;
  std::tie(theta, phi) = dir.to_euler();

  return Vec2(theta / (2*PI), phi / M_PI);
}

Vec3 from_euler_uv(const Vec2& uv)
{
  return Vec3::from_euler(uv.u * 2 * M_PI, uv.v * M_PI);
}

scalar clamp(scalar x, scalar m, scalar M)
{
  return std::min(M, std::max(x, m));
}

scalar p5_interp(scalar x)
{
  return x * x * x * (10 + x * (-15 + x * 6));
}

scalar p3_interp(scalar x)
{
  return x * x * (-2 + x + 3);
}

Vec3 lerp(const Vec3& a, const Vec3& b, const Vec3& t)
{
  return Vec3{lerp(a.x, b.x, t.x), lerp(a.y, b.y, t.y), lerp(a.z, b.z, t.z)};
}

scalar unitize(scalar x)
{
  return x * 0.5 + 0.5;
}

scalar norm(const scalar& s)
{
  return fabs(s);
}

scalar fresnel_reflectance_schlick(const Vec3& incoming, const Vec3& half,
                                   scalar nr)
{
  scalar r0 = schlick_r0_term(nr);
  scalar d = std::abs(incoming.dot(half));
  scalar ft = 1 - d;
  ft *= ft;
  ft *= ft;
  ft *= (1-d);  // ft = (1-d)^5;
  scalar r = r0 + (1 - r0)*ft;

  return r;
}
scalar fresnel_transmittance_schlick(const Vec3& incoming, const Vec3& half,
                                     scalar nr)
{
  return 1.0 - fresnel_reflectance_schlick(incoming, half, nr);
}

scalar fresnel_reflectance(const Vec3& incoming, const Vec3& normal,
                           scalar nr)
{
  const scalar ci = incoming.dot(normal);
  const scalar si2 = 1 - ci * ci;
  
  const scalar u = sqrt( 1 - nr * nr * si2 );

  const scalar sq_Rs = (nr * ci - u) / (nr * ci + u);
  const scalar sq_Rp = (nr * u - ci) / (nr * u + ci);

  return 0.5 * (sq_Rs * sq_Rs + sq_Rp * sq_Rp);
}

scalar fresnel_transmittance(const Vec3& incoming, const Vec3& normal,
                             scalar nr)
{
  return 1 - fresnel_reflectance(incoming, normal, nr);
}

bool has_tir(const Vec3& incoming, const Vec3& normal, scalar nr)
{
  scalar cosi = -incoming.dot(normal);
  scalar sin2t = nr*nr*(1 - cosi*cosi);

  return sin2t < 1;
}

Vec3 refraction_direction(const Vec3& incoming, const Vec3& normal, scalar nr)
{
  scalar cosi = -incoming.dot(normal);
  scalar sin2t = nr*nr*(1 - cosi*cosi);
  scalar cos2t = 1 - sin2t;
  if (cos2t < 0)
    return Vec3::zero;
  return (-incoming * nr - (nr * cosi + sqrt(cos2t))*normal).normal();
}


Vec3 interpolate_quadratic(scalar x1, scalar y1, scalar x2, scalar y2, scalar x3, scalar y3)
{
  Mat33 vandermonde({x1*x1, x1, 1, x2*x2, x2, 1, x3*x3, x3, 1});
  auto inv = vandermonde.inverse();
  auto y = Vec3{y1, y2, y3};
  return inv * y;
}

void convolve(uint w, uint h, const vector<scalar>& data,
              uint fw, uint fh, const vector<scalar>& filter,
              vector<scalar>& conv)
{
  vector<scalar> conv_output(data.size());
  if (data.size() == 0)
    return;
  if (filter.size() == 0)
    return;

  assert(data.size() == w * h);
  assert(filter.size() == fw * fh);

  uint hfh = (fh - 1) / 2; // half of filter height
  uint hfw = (fw - 1) / 2; // half of filter width

  scalar center_filter_value = filter[fw * hfh + hfw];

  // initialize conv with the center part of the data
  conv_output.resize(data.size());
  transform(data.begin(), data.end(), conv_output.begin(),
            [=](scalar x) { return x * center_filter_value; });

  int fi = 0;

  for (uint fy = 0; fy < fh; ++fy)
  {
    for (uint fx = 0; fx < fw; ++fx, ++fi)
    {
      // skip the center, since we've already accounted for that contribution
      if (fx == hfw && fy == hfh)
        continue;
      const scalar fv = filter[fi];
      if (fv == 0)
        continue;

      // center coordinates
      int cy = hfh-fy, cx = hfw - fx;

      const int my = std::max<int>(0, cy);
      const int My = h + std::min<int>(0, cy);

      const int mx = std::max<int>(0, cx);
      const int Mx = w + std::min<int>(0, cx);

      for (int y = my; y < My; ++y)
      {
        for (int x = mx; x < Mx; ++x)
        {
          conv_output[y*w+x] += fv * data[(y-cy)*w + (x-cx)];
        }
      }
    }
  }
  conv = std::move(conv_output);
}
