#include "bounds.h"

namespace bounds
{
  using std::ostream;
  ostream& operator<<(ostream& out, const bounds::AABB& b)
  {
    out << "{" << b.min << "} - {" << b.max << "}";
    return out;
  }
}
