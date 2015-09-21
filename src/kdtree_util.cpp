#include "kdtree_util.h"

namespace kd
{
  split_plane::split_plane(scalar s, NodeAxis a) : split(s), axis(a)
  {
  }

  bool split_plane::operator <(const split_plane& rhs) const
  {
    return split < rhs.split;
  }

  ostream& operator <<(ostream& out, const split_plane& sp)
  {
    return out << "SPLIT(" << (sp.axis == X ? "X" : (sp.axis == Y ? "Y" : "Z")) << " = " << sp.split << ")";
  }
}
