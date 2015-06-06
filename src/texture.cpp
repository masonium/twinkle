#include "texture.h"

spectrum GridTexture2D::at_point(const Intersection& isect) const
{
  scalar xf = fmod(isect.u * grid_size, 1.0);
  scalar yf = fmod(isect.v * grid_size, 1.0);
  
  if (min(xf, yf) < border_pct* 0.5 || max(xf, yf) > (1 - border_pct * 0.5))
    return border;
  else
    return solid;
}
