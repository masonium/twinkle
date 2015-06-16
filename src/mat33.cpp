#include "vec3.h"

scalar Mat33::det() const
{
  return v[0] * (v[4]*v[8] - v[5]*v[7])
    - v[1] * (v[5]*v[6] - v[3]*v[8])
    + v[2] * (v[3]*v[7] - v[4]*v[6]);
}

Mat33 Mat33::inverse() const
{
  auto d = det();
  return Mat33({(v[4]*v[8] - v[5]*v[7]) / d,
        (v[5]*v[6] - v[3]*v[8]) / d,
        (v[3]*v[7] - v[4]*v[6]) / d,
        (v[7]*v[2] - v[8]*v[1]) / d,
        (v[8]*v[0] - v[6]*v[2]) / d,
        (v[6]*v[1] - v[7]*v[0]) / d,
        (v[1]*v[5] - v[2]*v[4]) / d,
        (v[2]*v[3] - v[0]*v[5]) / d,
        (v[0]*v[4] - v[1]*v[3]) / d
        });
}
