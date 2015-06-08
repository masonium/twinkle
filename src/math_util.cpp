#include "math_util.h"
#include "vec3.h"

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
