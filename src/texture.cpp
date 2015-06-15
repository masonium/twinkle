#include "texture.h"

spectrum SolidColor::at_point(const Intersection& isect) const
{
  return color;
}

spectrum SolidColor::at_coord(scalar u, scalar v) const
{
  return color;
}

spectrum GridTexture2D::at_coord(scalar u, scalar v) const
{
  scalar xf = fmod(u * grid_size, 1.0);
  scalar yf = fmod(v * grid_size, 1.0);
  
  if (min(xf, yf) < border_pct * 0.5 || max(xf, yf) > (1 - border_pct * 0.5))
    return border;
  else
    return solid;
}
