#include "texture.h"
#include "intersection.h"

spectrum Texture2D::at_point(const IntersectionView& isect) const
{
  return at_coord(isect.tc);
}
