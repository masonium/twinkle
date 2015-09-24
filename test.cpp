#include "shapes/sphere.h"
#include "scene.h"
#include "bsdf.h"
#include "math_util.h"
#include "camera.h"
#include "sampler.h"
#include "spectrum.h"

#include "tests/test_util.h"

namespace
{

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
    BasicScene scene;
    shared_ptr<Sphere> sphere;
    Ray ray;
    shared_ptr<Shape> shape;
  };

  TEST_FIXTURE(SingleSphereFixture, scene_test)
  {
    auto isect = scene.intersect(ray);

    CHECK(isect.is());
  }

  TEST_FIXTURE(SingleSphereFixture, sphere)
  {
    auto isect = scene.intersect(ray);

    CHECK(isect.is());

    if (isect.is())
    {
      CHECK_CLOSE(isect.get().t, 2.0, EPS);
      CHECK_CLOSE(sphere->intersect(ray).get(), 2.0, EPS);
    }
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
    ConstSampler sampler{0.5, 0.5};
    Ray r = cam.sample_pixel(1, 1, 0, 0, sampler).ray;
    r.normalize();
    CHECK_CLOSE(1.0, r.direction.norm(), PRECISE_EPS);
    CHECK_ARRAY_CLOSE(Vec3(0.0, 0.0, -1.0).v, r.direction.v, 3, PRECISE_EPS);
  }

  TEST(TwoPixelCamera)
  {
    PerspectiveCamera cam(Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0, 1.0);
    ConstSampler sampler{0.5, 0.5};
    Ray r1 = cam.sample_pixel(2, 2, 0, 0, sampler).ray;
    Ray r2 = cam.sample_pixel(2, 2, 0, 1, sampler).ray;
    Ray r3 = cam.sample_pixel(2, 2, 1, 0, sampler).ray;
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

  SUITE(Triangle)
  {
    TEST (isect)
    {
      {
        auto t = ray_triangle_intersection(Ray(Vec3(0.25, 0.25, 1), Vec3(0, 0, -1)),
                                           Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 1, 0));

        CHECK_CLOSE(1.0, t.get(), PRECISE_EPS);
      }
      {
        Vec3 target_dir = Vec3(-2.25, -0.25, 0);
        auto t = ray_triangle_intersection(Ray(Vec3(2, 0.5, 0), target_dir.normal()),
                                           Vec3(0, 0, 1.0), Vec3(0, 0, -1.0), Vec3(-0.5, 0.5, 0));

        CHECK_CLOSE(target_dir.norm(), t.get(), PRECISE_EPS);
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
