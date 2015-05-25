#include "spectrum.h"

const spectrum spectrum::zero{0.0};
const spectrum spectrum::one{1.0};

ostream&  operator<<(ostream& out, spectrum s)
{
  out << "{" << s.x << ", " << s.y << ", " << s.z << "}";
  return out;
}

