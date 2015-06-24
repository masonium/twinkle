#include "math_util.h"
#include "vec3.h"
#include "mat33.h"
#include "ray.h"

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

scalar ray_triangle_intersection(const Ray& ray, const Vec3& v1, const Vec3& v2, const Vec3& v3,
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
