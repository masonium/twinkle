#include "test_util.h"
#include "math_util.h"

namespace
{
  TEST(inverse)
  {
    Mat33 eye = Mat33::identity;
    CHECK_MAT(eye, eye.inverse());
    for (int i = 0; i < 20; ++i)
    {
      scalar angle = rf() * 2 * PI;
      auto axis = random_normal();
      Mat33 r = Mat33::from_axis_angle(axis, angle);

      CHECK_MAT(eye, r * r.inverse());
    }
  }
  TEST(determinant)
  {
    Mat33 eye = Mat33::identity;
    CHECK_CLOSE(1.0, eye.det(), EPS);

    for (int i = 0; i < 20; ++i)
    {
      scalar angle = rf() * 2 * PI;
      auto axis = random_normal();
      Mat33 r = Mat33::from_axis_angle(axis, angle);
      CHECK_CLOSE(1.0, r.det(), EPS);
    }
  }

}
