#include <iostream>
#include "geometry/implicit.h"
#include "test_util.h"
#include "geometry/sphere.h"

scalar sphere_sdf(const Vec3& p)
{
  return p.norm() - 1.0;
}

auto sphere_grad = gradient_from_sdf(sphere_sdf);

TEST(sphere_implicit)
{
  auto bbox = bounds::AABB(Vec3{-1.1}, Vec3{1.1});
  ImplicitSurface imp(sphere_sdf, sphere_grad, 1.0, bbox);
  Sphere sp(Vec3::zero, 1.0);

  UniformSampler samp;
  for (int i = 0; i < 100; ++i)
  {
    scalar A = rf() * 2 + 0.1;

    Vec3 n = cosine_weighted_hemisphere_sample(samp.sample_2d());

    Ray ray(Vec3::z_axis * -1.0 * (1.0 + A), n);

    scalar crit_angle = asin(1.0 / (1.0 + A));

    scalar angle = acos(n.dot(Vec3::z_axis));

    auto imp_t = imp.intersect(ray);
    auto sp_t = sp.intersect(ray);

    if (angle > crit_angle+0.001)
    {
      CHECK(!imp_t.is());
      CHECK(!sp_t.is());
    }
    if (angle < crit_angle - 0.001)
    {
      scalar a = 1.0, b = -2 * n.z * (A + 1.0);
      scalar c = (1.0 + A) * (1.0 + A) - 1;

      scalar t_analytic = qf(a, b, c);

      CHECK_CLOSE(t_analytic, imp_t.get(), EPS);
      CHECK_CLOSE(t_analytic, sp_t.get(), EPS);
    }
  }
}
