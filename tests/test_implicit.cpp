#include <iostream>
#include "implicit.h"
#include "test_util.h"

scalar sphere_sdf(const Vec3& p)
{
  return p.norm() - 1.0;
}

auto sphere_grad = gradient_from_sdf(sphere_sdf);

TEST(sphere_implicit)
{
  ImplicitSurface imp(sphere_sdf, sphere_grad, 1.0);
  UniformSampler samp;
  for (int i = 0; i < 100; ++i)
  {
    Vec3 n = cosine_weighted_hemisphere_sample(samp.sample_2d());
    if (n.z > 0)
      n = -n;
    Ray r(Vec3::z_axis * sqrt(3), n);

    scalar t = imp.intersect(r, SCALAR_MAX);
    scalar angle = acos(n.dot(-Vec3::z_axis));
    if (angle < PI/6.0-0.1)
      CHECK(t > 0);
    else if (angle > PI/6.0+0.1)
      CHECK(t < 0);
  }
}
