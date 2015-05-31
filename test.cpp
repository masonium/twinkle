#include "sphere.h"
#include "scene.h"
#include "bsdf.h"
#include "math_util.h"
#include "camera.h"
#include "sampler.h"
#include "spectrum.h"

#include "unittest++/UnitTest++.h"

#define EPS 0.0001
#define PRECISE_EPS 0.00001

scalar rf()
{
  return rand() / (1.0 + RAND_MAX);
}

// random uniform symmetric
scalar rus()
{
  return rf() * 2.0 - 1.0;
}

Vec3 random_normal()
{
  return Vec3{rus(), rus(), rus()}.normal();
}

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
  SingleSphereFixture() : d{1.0}, sphere{Vec3{0.0, 0.0, 0.0}, 1.0},
                          ray{Vec3{5.0, 0.0, 0.0}, Vec3{-2.0, 0.0, 0.0}},
                          shape{&sphere, &d, new SolidColor(spectrum{1.0})}
  {
    scene.add(&shape);
  }
  Diffuse d;
  Scene scene;
  Sphere sphere;
  Ray ray;
  Shape shape;
};

TEST_FIXTURE(SingleSphereFixture, scene_test)
{
  Intersection isect = scene.intersect(ray);

  CHECK(isect.valid());
  CHECK_EQUAL(isect.shape, &shape);

}

TEST_FIXTURE(SingleSphereFixture, sphere)
{
  Intersection isect = scene.intersect(ray);

  CHECK_CLOSE(isect.t, 2.0, EPS);
  CHECK_CLOSE(sphere.intersect(ray), 2.0, EPS);
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

int main(int argc, char** args)
{
  UnitTest::RunAllTests();
}
