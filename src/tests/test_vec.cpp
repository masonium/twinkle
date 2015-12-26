#include "test_util.h"
#include "math_util.h"
#include "vec2.h"

namespace
{

    SUITE(Vec3)
  {
    class VecFixture
    {
    public:
      VecFixture() : x{0.0, 1.0, 2.0}, y{1.0}, z{} {}

      Vec3 x, y, z;
    };


    TEST_FIXTURE(VecFixture, Vec3_add)
    {
      CHECK_ARRAY_CLOSE((x+y).v, Vec3(1.0, 2.0, 3.0).v, 3, EPS);
      CHECK_ARRAY_CLOSE((x+z).v, Vec3(0.0, 1.0, 2.0).v, 3, EPS);
    }

    TEST_FIXTURE(VecFixture, Vec3_sub)
    {
      CHECK_ARRAY_CLOSE((x-y).v, Vec3(-1.0, 0.0, 1.0).v, 3, EPS);
      CHECK_ARRAY_CLOSE((x-z).v, Vec3(0.0, 1.0, 2.0).v, 3, EPS);
    }

    TEST_FIXTURE(VecFixture, Vec3_scalar_mult)
    {
      CHECK_ARRAY_CLOSE( (x * 2.0).v, Vec3(0.0, 2.0, 4.0).v, 3, PRECISE_EPS);
    }

    TEST_FIXTURE(VecFixture, Vec3_scalar_div)
    {
      CHECK_ARRAY_CLOSE( (x / 2.0).v, Vec3(0.0, 0.5, 1.0).v, 3, PRECISE_EPS);
    }

    TEST(Vec3_rotate_aa_none)
    {
      for (int i = 0; i < 10; ++i)
      {
        Vec3 axis = random_normal();
        Vec3 target = random_normal() * (rf() + 0.01) * 5.0;
        CHECK_ARRAY_CLOSE(target.v, target.rotateAxisAngle(axis, 0).v,
                          3, PRECISE_EPS);
      }
    }

    TEST(Vec3_rotate_aa_180)
    {
      const Vec3 z_axis{0.0, 0.0, 1.0};
      for (int i = 0; i < 10; ++i)
      {
        Vec3 target = Vec3{rus(), rus(), 1.0} * (rf() + 0.01) * 5.0;
        CHECK_ARRAY_CLOSE(Vec3(-target.x, -target.y, target.z).v,
                          target.rotateAxisAngle(z_axis, PI).v,
                          3, PRECISE_EPS);
      }
    }

    TEST(Vec3_rotate_aa)
    {
      {
        Vec3 a{1.0, 1.0, 0.0};
        CHECK_ARRAY_CLOSE(Vec3(-1.0, 1.0, 0.0).v,
                          a.rotateAxisAngle(Vec3(0.0, 0.0, 1.0), PI*0.5).v,
                          3, EPS);
      }

      {
        Vec3 b{1.0, 1.0, 1.0};
        CHECK_ARRAY_CLOSE(Vec3(-1.0, 1.0, 1.0).v,
                          b.rotateAxisAngle(Vec3(0.0, 0.0, 1.0), PI*0.5).v,
                          3, EPS);
      }

    }

    TEST(Vec_assign)
    {
      Vec3 x{2.0, 4.0, 5.0};
      Vec3 y = x;
      CHECK_ARRAY_CLOSE(x.v, y.v, 3, PRECISE_EPS);
    }

    TEST(Vec_ref)
    {
      Vec3 x{2.0, 4.0, 5.0};
      Vec3& y = x;
      CHECK_ARRAY_CLOSE(x.v, y.v, 3, PRECISE_EPS);
    }

    TEST(Vec_rval)
    {
      Vec3 x{2.0, 4.0, 5.0};
      Vec3 y = x;
      Vec3 z = move(x);
      CHECK_ARRAY_CLOSE(y.v, z.v, 3, PRECISE_EPS);
    }

    TEST(Vec_angle)
    {
      for (int i = 0; i < 20; ++i)
      {
        scalar theta = rf() * 2 * PI;
        scalar phi = rf() * PI;
        Vec3 x = Vec3::from_euler(theta, phi);
        scalar t2, p2;
        x.to_euler(t2, p2);
        CHECK_CLOSE(theta, t2, EPS);
        CHECK_CLOSE(phi, p2, EPS);
      }
    }

    TEST(Vec_from_to_euler_uv)
    {
      for (int i = 0; i < 20; ++i)
      {
        auto v = random_normal();
        CHECK_VEC(from_euler_uv(to_euler_uv(v)), v);
      }
    }
    TEST(Vec_to_from_euler_uv)
    {
      for (int i = 0; i < 20; ++i)
      {
        auto uv = Vec2(rf(), rf());
        CHECK_VEC2(to_euler_uv(from_euler_uv(uv)), uv);
      }
    }
  }
}
