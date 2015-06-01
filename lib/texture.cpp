#include "texture.h"

spectrum GridTexture2D::at_point(const Intersection& isect) const
{
  scalar g = min(fmod(isect.u * grid_size, 1.0), fmod(isect.v * grid_size, 1.0));
  
  if (g < border_pct)
    return border;
  else
    return solid;
}
