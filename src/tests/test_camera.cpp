#include "tests/test_util.h"
#include "camera.h"

namespace test
{
  TEST(Camera)
  {
    PerspectiveCamera cam(Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0);

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
    PerspectiveCamera cam(Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0);
    ConstSampler sampler{0.5, 0.5};
    Ray r = cam.sample_pixel(1, 1, 0, 0, sampler).ray;
    r.normalize();
    CHECK_CLOSE(1.0, r.direction.norm(), PRECISE_EPS);
    CHECK_ARRAY_CLOSE(Vec3(0.0, 0.0, -1.0).v, r.direction.v, 3, PRECISE_EPS);
  }

  TEST(TwoPixelCamera)
  {
    PerspectiveCamera cam(Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0);
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

}
