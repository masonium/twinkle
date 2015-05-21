#include "sphere.h"
#include "scene.h"
#include "bsdf.h"
#include "math_util.h"
#include "camera.h"

#include "unittest++/UnitTest++.h"

TEST(quad)
{
  CHECK_CLOSE(qf(1.0, -6.0f, 9.0f), 3.0, 0.000001);
  CHECK(qf(1.0, 6.0f, 9.0f) < 0);
  CHECK_CLOSE(qf(1.0f, 1.0f, -2.0f), 1.0f, 0.00001);
}

class SingleSphereFixture
{
public:
  SingleSphereFixture() : d{1.0}, sphere{Vec3{0.0, 0.0, 0.0}, 1.0},
    ray{Vec3{5.0, 0.0, 0.0}, Vec3{-2.0, 0.0, 0.0}},
    shape{&sphere, &d}
  {
    scene.add_shape(&shape);
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
  
  CHECK_CLOSE(isect.t, 2.0, 0.0001);
  CHECK_CLOSE(sphere.intersect(ray), 2.0, 0.0001);
}

TEST(cross_product)
{
  Vec3 x{1, 0, 0};
  Vec3 y{0, 1, 0};
  Vec3 z{0, 0, 1};

  CHECK_ARRAY_CLOSE(z.v, x.cross(y).v, 3, 0.00001);
}

TEST(dot_product)
{
  Vec3 x{sqrt(2), -sqrt(2), sqrt(12)};
  CHECK_CLOSE(x.norm(), 4.0, 0.000001);
}  

TEST(pi)
{
  CHECK_CLOSE(PI, 3.14159, 0.0001);
}

TEST(Camera)
{
  Camera cam {Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0, 1.0};

  const Vec3 fn = cam.aspect_forward.normal();
  const Vec3 up = cam.up;
  CHECK_ARRAY_CLOSE(Vec3(0, 0, -1).v, fn.v, 3, 0.00001);
  CHECK_ARRAY_CLOSE(Vec3(0, 1, 0).v, up.v, 3, 0.00001);
  CHECK_ARRAY_CLOSE(Vec3(1, 0, 0).v, cam.right.v, 3, 0.00001);
}

TEST(SinglePixelCamera)
{
  Camera cam {Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0, 1.0};
  Film f(1, 1);
  Ray r = cam.sample_pixel_ray(&f, 0, 0);
  r.normalize();
  CHECK_CLOSE(r.direction.norm(), 1.0, 0.00001);
  CHECK_ARRAY_CLOSE(r.direction.v, Vec3(0.0, 0.0, -1.0).v, 3, 0.00001);
}

TEST(TwoPixelCamera)
{
  Camera cam {Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0, 1.0};
  Film f(2, 2);
  Ray r1 = cam.sample_pixel_ray(&f, 0, 0);
  Ray r2 = cam.sample_pixel_ray(&f, 0, 1);
  Ray r3 = cam.sample_pixel_ray(&f, 1, 0);  
  r1.normalize();
  r2.normalize();
  r3.normalize();
  
  CHECK_CLOSE(r1.direction.x, r2.direction.x, 0.00001);
  CHECK_CLOSE(r1.direction.y, -r2.direction.y, 0.00001);
  CHECK_CLOSE(r1.direction.z, r2.direction.z, 0.00001);
  
  CHECK_CLOSE(-r1.direction.x, r3.direction.x, 0.00001);
  CHECK_CLOSE(r1.direction.y, r3.direction.y, 0.00001);
  CHECK_CLOSE(r1.direction.z, r3.direction.z, 0.00001);
}

int main(int argc, char** args)
{
  UnitTest::RunAllTests();
}
