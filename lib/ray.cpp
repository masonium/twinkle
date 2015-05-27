#include "ray.h"
#include <cmath>

Ray Ray::nudge(scalar epsilon) const
{
  return Ray(position + direction.normal() * epsilon, direction);
}

