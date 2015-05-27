#include "ray.h"
#include <cmath>

Ray Ray::nudge(scalar epsilon) const
{
  return Ray(position + direction.normal() * epsilon, direction);
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
