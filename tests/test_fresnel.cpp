#include "test_util.h"
#include "math_util.h"

TEST(refract)
{
  scalar n1 = 1.1;
  scalar n2 = 1.5;
  for (int i = 0; i < 20; ++i)
  {
    Vec3 x = random_normal();
    Vec3 y = random_normal();
    Vec3 refrac = refraction_direction(x, y, n1, n2);
    CHECK_CLOSE(0, refrac.dot(x.cross(y)), PRECISE_EPS);
    CHECK_CLOSE(1.0, refrac.norm(), PRECISE_EPS);
    CHECK_CLOSE(n1 * x.cross(y).norm(), n2 * refrac.cross(y).norm(), PRECISE_EPS);
  }
}

TEST(reflect)
{
  for (int i = 0; i < 20; ++i)
  {
    Vec3 x = random_normal();
    Vec3 y = random_normal();
    Vec3 refl = x.reflect_over(y);
    CHECK_CLOSE(x.dot(y), refl.dot(y), PRECISE_EPS);
    CHECK_VEC(x.cross(y), y.cross(refl));
  }
}
