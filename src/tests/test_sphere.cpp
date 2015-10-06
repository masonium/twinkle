#include "tests/test_util.h"
#include "geometry/sphere.h"
#include "scene.h"

namespace
{
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
      CHECK_CLOSE(isect.get().t(), 2.0, EPS);
      CHECK_CLOSE(sphere->intersect(ray).get(), 2.0, EPS);
    }
  }
}
