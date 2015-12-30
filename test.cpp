#include "geometry/sphere.h"
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
    class TonemapFixture
    {
    public:
      TonemapFixture() : img(1, 1)
      {
      }

      Array2D<spectrum> img;
    };

    TEST_FIXTURE(TonemapFixture, LinearNoOp)
    {
      LinearToneMapper tm;
      spectrum val{0.2, 0.5, 0.3};
      img(0, 0) = val;
      auto image = tm.tonemap(img);
      CHECK_VEC(val, img(0, 0));
    }

    TEST_FIXTURE(TonemapFixture, Linear)
    {
      LinearToneMapper tm;
      spectrum val{0.2, 2.0, 0.3};
      spectrum lin_val{0.1, 1.0, 0.15};
      img(0, 0) = val;
      auto image = tm.tonemap(img);
      CHECK_VEC(lin_val, image(0, 0));
    }
    TEST_FIXTURE(TonemapFixture, Cutoff)
    {
      CutoffToneMapper tm;
      spectrum val{0.2, 2.0, 0.3};
      spectrum cut_val{0.2, 1.0, 0.3};
      img(0, 0) = val;
      auto image = tm.tonemap(img);
      CHECK_VEC(cut_val, image(0, 0));
    }
  }
}

int main(int argc, char** args)
{
  UnitTest::RunAllTests();
}
