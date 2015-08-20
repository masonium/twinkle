#include "math_util.h"
#include "test_util.h"

scalar eval_quadratic(Vec3 coef, scalar x)
{
  return (coef[0] * x + coef[1]) * x + coef[2];
}

namespace
{
  TEST(quadratic_interpolate)
  {
    for (int i = 0; i < 100; ++i)
    {
      scalar x = rus();
      scalar y = x + rf() + 1;
      scalar z = y + rf() + 1;

      Vec3 coef{ rus()*10, rus()*20, rus() * 30 };

      CHECK_VEC( coef, interpolate_quadratic(x, eval_quadratic(coef, x),
                                             y, eval_quadratic(coef, y),
                                             z, eval_quadratic(coef, z)));
    }
  }
}
