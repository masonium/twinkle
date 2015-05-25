#include "spectrum.h"

const spectrum spectrum::zero{0.0};
const spectrum spectrum::one{1.0};

spectrum spectrum::max(const spectrum &a, const spectrum& b)
{
  return spectrum{::max(a.x, b.x), ::max(a.y, b.y), ::max(a.z, b.z)};
}


ostream& operator<<(ostream& out, spectrum s)
{
  out << "{" << s.x << ", " << s.y << ", " << s.z << "}";
  return out;
}

