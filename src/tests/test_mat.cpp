#include "test_util.h"
#include "math_util.h"
#include "mat33.h"

namespace
{
  SUITE(Mat)
  {
    TEST(transpose)
    {
      Mat33 x{1, 2, 3, 4, 5, 6, 7, 8, 9};
      Mat33 xt{1, 4, 7, 2, 5, 8, 3, 6, 9};
      CHECK_MAT(xt, x.transpose());
    }

    TEST(rotate_z)
    {
      Mat33 r1 = Mat33::rotate_match( Vec3(0.5, 0.5, sqrt(0.5)), Vec3::z_axis );
      Mat33 r2 = Mat33::rotate_to_z( Vec3(0.5, 0.5, sqrt(0.5)) );
      CHECK_MAT(r1, r2);
    }

    TEST(rotate_rand)
    {
      for (int i = 0; i < 100; ++i)
      {
        Vec3 t1 = random_normal();
        Vec3 t2 = random_normal();
        Mat33 r1 = Mat33::rotate_match( t1, t2);
        CHECK_VEC( t2, r1 * t1 );
      }
    }

    TEST(rotate)
    {
      Mat33 r1 = Mat33::rotate_match( Vec3::x_axis, Vec3::y_axis );
      Mat33 r2 = Mat33::from_axis_angle( Vec3::z_axis, PI/2.0 );
      CHECK_MAT(r1, r2);
      CHECK_VEC(Vec3::y_axis, r1 * Vec3::x_axis);

      {
        Vec3 target(sqrt(0.5), sqrt(0.5), 0);
        Mat33 r1 = Mat33::rotate_match( Vec3::x_axis, target );
        Mat33 r2 = Mat33::from_axis_angle( Vec3::z_axis, PI/4.0 );
        CHECK_MAT(r1, r2);
        CHECK_VEC(target, r1 * Vec3::x_axis);
      }
    }

    TEST(rotate_z_to_z)
    {
      Mat33 eye = Mat33::identity;
      Mat33 m = Mat33::rotate_match(Vec3::z_axis, Vec3::z_axis);
      CHECK_MAT(eye, m);
    }
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
}
