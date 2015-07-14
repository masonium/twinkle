#include "math_util.h"
#include "vec3.h"
#include "mat33.h"
#include "ray.h"
#include <vector>
#include <cassert>
#include <algorithm>

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
scalar unitize(scalar x)
{
  return x * 0.5 + 0.5;
}

scalar norm(const scalar& s)
{
  return fabs(s);
}



scalar fresnel_reflectance(const Vec3& incoming, const Vec3& normal,
                           scalar n1, scalar n2)
{
  const scalar ci = incoming.dot(normal);
  const scalar si2 = 1 - ci * ci;

  const scalar nr = n1/n2;
  
  const scalar u = sqrt( 1 - nr * nr * si2 );

  const scalar sq_Rs = (nr * ci - u) / (nr * ci + u);
  const scalar sq_Rp = (nr * u - ci) / (nr * u + ci);

  return 0.5 * (sq_Rs * sq_Rs + sq_Rp * sq_Rp);
}

scalar fresnel_transmittance(const Vec3& incoming, const Vec3& normal,
                             scalar n1, scalar n2)
{
  return 1 - fresnel_reflectance(incoming, normal, n1, n2);
}

Vec3 refraction_direction(const Vec3& incoming, const Vec3& normal,
                          scalar n1, scalar n2)
{
  scalar nr = n1 / n2;

  scalar cosi = -incoming.dot(normal);
  scalar sin2t = nr*nr*(1 - cosi*cosi);
  return (-incoming * nr - (nr * cosi + sqrt(1 - sin2t))*normal).normal();
}

scalar ray_triangle_intersection(const Ray& ray,
                                 const Vec3& v1, const Vec3& v2, const Vec3& v3,
                                 scalar max_t)
{
  Vec3 e1 = v2 - v1, e2 = v3 - v1;
  Vec3 p = ray.direction.cross(e2);
  scalar det = e1.dot(p);
  if (fabs(det) < EPSILON)
    return -1;

  const scalar inv_det = 1 / det;

  Vec3 corner_to_ray = ray.position - v1;
  Vec3 q = corner_to_ray.cross(e1);

  scalar t = q.dot(e2) * inv_det;
  if (t > max_t || t < EPSILON)
    return -1;

  scalar u = corner_to_ray.dot(p) * inv_det;
  if (u < 0 || u > 1)
    return -1;

  scalar v = q.dot(ray.direction) * inv_det;
  if (v < 0 || u + v > 1)
    return -1;

  return t;
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
