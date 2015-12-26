#include "test_util.h"
#include "bounds.h"
#include "geometry/box.h"

namespace
{
  TEST(box_axis)
  {
    Ray r(Vec3{2, 0, 0}, Vec3{-1, 0, 0});
    bounds::AABB box{Vec3{-1, -1, -1}, Vec3{1, 1, 1}};

    scalar t0, t1;
    bool isect = box.intersect(r, t0, t1);

    CHECK(isect);
    CHECK_CLOSE(1.0, t0, EPS);
    CHECK_CLOSE(3.0, t1, EPS);
  }

  TEST(box_outside)
  {
    Ray r(Vec3{2, 2, 0}, Vec3{-1, 0, 0});
    bounds::AABB box{Vec3{-1, -1, -1}, Vec3{1, 1, 1}};

    scalar t0, t1;
    bool isect = box.intersect(r, t0, t1);
    CHECK(!isect);
  }

  TEST(box_normal)
  {
    Box b(bounds::AABB(Vec3(0.0), Vec3(1.0)));

    CHECK_VEC(b.normal(0, Vec3{0.0, 0.3, 0.5}), -Vec3::x_axis);
    CHECK_VEC(b.normal(0, Vec3{0.3, 0.0, 0.5}), -Vec3::y_axis);
    CHECK_VEC(b.normal(0, Vec3{0.3, 0.5, 0.0}), -Vec3::z_axis);

    CHECK_VEC(b.normal(0, Vec3{1.0, 0.3, 0.5}), Vec3::x_axis);
    CHECK_VEC(b.normal(0, Vec3{0.3, 1.0, 0.5}), Vec3::y_axis);
    CHECK_VEC(b.normal(0, Vec3{0.3, 0.5, 1.0}), Vec3::z_axis);
  }

  TEST(box_middle)
  {
    bounds::AABB box{Vec3{-1, -1, -1}, Vec3{1, 1, 1}};

    for (int i = 0; i < 50; ++i)
    {
      Vec3 n = random_normal();
      Ray r(Vec3::zero, n);
      scalar t0, t1;
      bool isect = box.intersect(r, t0, t1);
      CHECK(isect);
      CHECK(t1 >= 1.0);
      CHECK(t1 < sqrtf(3));
      CHECK_CLOSE(-t0, t1, EPS);
    }
  }
};
