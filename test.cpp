#include "shapes/sphere.h"
#include "scene.h"
#include "bsdf.h"
#include "math_util.h"
#include "camera.h"
#include "sampler.h"
#include "spectrum.h"

#include "test_util.h"

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
  }

  SUITE(spectrum)
  {
    TEST(spectrum_hsv)
    {
      spectrum red{1.0, 0.0, 0.0};
      CHECK_ARRAY_CLOSE(red.v, spectrum::from_hsv(0.0, 1.0, 1.0).v, 3, EPS);
      spectrum yellow{1.0, 1.0, 0.0};
      CHECK_ARRAY_CLOSE(yellow.v, spectrum::from_hsv(60.0, 1.0, 1.0).v, 3, EPS);
    }
  }

  TEST(halton_1d_base2)
  {
    HaltonSampler samp;
    const double eps = EPS;

    const vector<scalar> first_samples_1d = {0.5, 0.25, 0.75, 0.125, 0.625, 0.375, 0.875};

    for (size_t i = 0; i < first_samples_1d.size(); ++i)
      CHECK_CLOSE(first_samples_1d[i], samp.sample_1d(), eps);
  }

  TEST(quad)
  {
    CHECK_CLOSE(qf(1.0, -6.0f, 9.0f), 3.0, 0.000001);
    CHECK(qf(1.0, 6.0f, 9.0f) < 0);
    CHECK_CLOSE(qf(1.0f, 1.0f, -2.0f), 1.0f, PRECISE_EPS);
  }

  class SingleSphereFixture
  {
  public:
    SingleSphereFixture() : sphere(make_shared<Sphere>(Vec3{0.0, 0.0, 0.0}, 1.0)),
                            ray{Vec3{5.0, 0.0, 0.0}, Vec3{-2.0, 0.0, 0.0}},
                            shape(make_shared<Shape>(sphere,
                                                     make_shared<RoughColorMaterial>(0, spectrum{1.0})))
    {
      scene.add(shape);
    }
    Scene scene;
    shared_ptr<Sphere> sphere;
    Ray ray;
    shared_ptr<Shape> shape;
  };

  TEST_FIXTURE(SingleSphereFixture, scene_test)
  {
    Intersection isect = scene.intersect(ray);

    CHECK(isect.valid());
  }

  TEST_FIXTURE(SingleSphereFixture, sphere)
  {
    Intersection isect = scene.intersect(ray);

    CHECK_CLOSE(isect.t, 2.0, EPS);
    CHECK_CLOSE(sphere->intersect(ray), 2.0, EPS);
  }

  TEST(cross_product)
  {
    Vec3 x{1, 0, 0};
    Vec3 y{0, 1, 0};
    Vec3 z{0, 0, 1};

    CHECK_ARRAY_CLOSE(z.v, x.cross(y).v, 3, PRECISE_EPS);
  }

  TEST(dot_product)
  {
    Vec3 x(sqrt(2), -sqrt(2), sqrt(12));
    CHECK_CLOSE(x.norm(), 4.0, 0.000001);
  }

  TEST(pi)
  {
    CHECK_CLOSE(PI, 3.14159, EPS);
  }

  TEST(Camera)
  {
    PerspectiveCamera cam(Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0, 1.0);

    const Vec3 pos = cam.position;
    const Vec3 fn = cam.aspect_forward.normal();
    const Vec3 up = cam.up;

    CHECK_ARRAY_CLOSE(Vec3(0, 0, 5).v, pos.v, 3, PRECISE_EPS);
    CHECK_ARRAY_CLOSE(Vec3(0, 0, 5).v, cam.position.v, 3, PRECISE_EPS);

    CHECK_ARRAY_CLOSE(Vec3(0, 0, -1).v, fn.v, 3, PRECISE_EPS);
    CHECK_ARRAY_CLOSE(Vec3(0, 1, 0).v, up.v, 3, PRECISE_EPS);
    CHECK_ARRAY_CLOSE(Vec3(1, 0, 0).v, cam.right.v, 3, PRECISE_EPS);
  }

  TEST(SinglePixelCamera)
  {
    PerspectiveCamera cam(Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0, 1.0);
    Film f(1, 1, new BoxFilter);
    Ray r = cam.sample_pixel(f, 0, 0, Sample5D{{0.5, 0.5, 0, 0, 0}}).ray;
    r.normalize();
    CHECK_CLOSE(1.0, r.direction.norm(), PRECISE_EPS);
    CHECK_ARRAY_CLOSE(Vec3(0.0, 0.0, -1.0).v, r.direction.v, 3, PRECISE_EPS);
  }

  TEST(TwoPixelCamera)
  {
    PerspectiveCamera cam(Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0, 1.0);
    Film f(2, 2, new BoxFilter);
    Sample5D samp{{0.5, 0.5, 0, 0, 0}};
    Ray r1 = cam.sample_pixel(f, 0, 0, samp).ray;
    Ray r2 = cam.sample_pixel(f, 0, 1, samp).ray;
    Ray r3 = cam.sample_pixel(f, 1, 0, samp).ray;
    r1.normalize();
    r2.normalize();
    r3.normalize();

    CHECK_CLOSE(r1.direction.x, r2.direction.x, PRECISE_EPS);
    CHECK_CLOSE(r1.direction.y, -r2.direction.y, PRECISE_EPS);
    CHECK_CLOSE(r1.direction.z, r2.direction.z, PRECISE_EPS);

    CHECK_CLOSE(-r1.direction.x, r3.direction.x, PRECISE_EPS);
    CHECK_CLOSE(r1.direction.y, r3.direction.y, PRECISE_EPS);
    CHECK_CLOSE(r1.direction.z, r3.direction.z, PRECISE_EPS);
  }

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
      Mat33 r1 = Mat33::rotate_match( Vec3{0.5, 0.5, sqrt(0.5)}, Vec3::z_axis );
      Mat33 r2 = Mat33::rotate_to_z( Vec3{0.5, 0.5, sqrt(0.5)} );
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
  }

  SUITE(Triangle)
  {
    TEST (isect)
    {
      {
        scalar t = ray_triangle_intersection(Ray(Vec3(0.25, 0.25, 1), Vec3(0, 0, -1)),
                                             Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 1, 0));

        CHECK_CLOSE(1.0, t, PRECISE_EPS);
      }
      {
        Vec3 target_dir = Vec3(-2.25, -0.25, 0);
        scalar t = ray_triangle_intersection(Ray(Vec3(2, 0.5, 0), target_dir.normal()),
                                             Vec3(0, 0, 1.0), Vec3(0, 0, -1.0), Vec3(-0.5, 0.5, 0));

        CHECK_CLOSE(target_dir.norm(), t, PRECISE_EPS);
      }
    }
  }

  SUITE(ToneMap)
  {
    TEST(LinearNoOp)
    {
      LinearToneMapper tm;
      spectrum val{0.2, 0.5, 0.3};
      vector<spectrum> image{val};
      tm.tonemap(image, image, 1, 1);
      CHECK_VEC(val, image[0]);
    }
    TEST(Linear)
    {
      LinearToneMapper tm;
      spectrum val{0.2, 2.0, 0.3};
      spectrum lin_val{0.1, 1.0, 0.15};
      vector<spectrum> image{val};
      tm.tonemap(image, image, 1, 1);
      CHECK_VEC(lin_val, image[0]);
    }
    TEST(Cutoff)
    {
      CutoffToneMapper tm;
      spectrum val{0.2, 2.0, 0.3};
      spectrum cut_val{0.2, 1.0, 0.3};
      vector<spectrum> image{val};
      tm.tonemap(image, image, 1, 1);
      CHECK_VEC(cut_val, image[0]);
    }
  }
}

int main(int argc, char** args)
{
  UnitTest::RunAllTests();
}
