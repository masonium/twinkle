#include "implicit.h"
#include "math_util.h"
#include "transformed.h"
#include <memory>

static const scalar MIN_STEP = 0.0001;

using std::make_shared;

ImplicitSurface::ImplicitSurface(ImplicitEvalFunc f_, ImplicitGradFunc g_,
                                 scalar lipschitz_const, const bounds::AABB& bounds)
  : f(f_), g(g_), L(lipschitz_const), bbox(bounds)
{

}


Vec3 ImplicitSurface::normal(SubGeo subgeo, const Vec3& point) const
{
  return g(point).normal();
}

/*
 * intersectino using a basic sphere-tracing algorithm
 */
scalar_fp ImplicitSurface::intersect(const Ray& r, scalar_fp max_t) const
{
  scalar t0, t1;
  if (!bbox.intersect(r, t0, t1))
    return sfp_none;

  if (max_t < t0 || max_t < MIN_STEP || t1 < MIN_STEP)
    return sfp_none;

  if (max_t.is())
    t1 = std::min(max_t.get(), t1);

  scalar t = MIN_STEP;

  scalar rdn = r.direction.norm();
  scalar dist = f(r.evaluate(t));

  if (fabs(dist) < MIN_STEP)
    return scalar_fp{t};


  const auto rdnl = rdn * L;
  do
  {
    scalar t_diff = std::max(dist, MIN_STEP) / rdnl;
    scalar new_t = t + t_diff;
    scalar new_dist = f(r.evaluate(new_t));
    if (fabs(new_dist) < MIN_STEP)
    {
      // If we find a near-enough surface point, use the secant approximation to
      // find the zero-point.
      return scalar_fp{t + (dist - 0) / (dist - new_dist) * t_diff};
    }

    t = new_t;
    dist = new_dist;
  } while (t < t1);

  return sfp_none;
}

class GradFromEval
{
public:
  GradFromEval(ImplicitEvalFunc f_) : f(f_)
  {
  }

  Vec3 grad(const Vec3& v)
  {
    scalar EPS = 0.0001;
    scalar p = f(v);
    return Vec3( (f(v + Vec3::x_axis * EPS) - p) / EPS,
                 (f(v + Vec3::y_axis * EPS) - p) / EPS,
                 (f(v + Vec3::z_axis * EPS) - p) / EPS);
  }

private:
  ImplicitEvalFunc f;
};

ImplicitGradFunc gradient_from_sdf(ImplicitEvalFunc f)
{
  auto grad = make_shared<GradFromEval>(f);
  return [=](const Vec3& v) { return grad->grad(v); };
}

////////////////////////////////////////////////////////////////////////////////

shared_ptr<Geometry> make_torus(Vec3 normal, scalar outer_radius, scalar inner_radius)
{
  auto sdf = [=](const Vec3& v) {
    scalar xz = sqrt(v.x*v.x + v.z*v.z);
    scalar a = xz - inner_radius;
    scalar b = v.y;
    return sqrt(a*a + b*b) - outer_radius;
  };

  auto gsdf = gradient_from_sdf(sdf);

  const auto Rxz = outer_radius + inner_radius;
  const auto lb = Vec3{-Rxz, -inner_radius, -Rxz};

  return make_shared<Transformed>(
    make_shared<ImplicitSurface>(sdf, gsdf, 1.0, bounds::AABB{lb, -lb}),
    Transform{Mat33::rotate_match(Vec3::y_axis, normal), Vec3::zero});
}

shared_ptr<Geometry> make_capsule(Vec3 direction, scalar length, scalar radius)
{
  Vec3 a = direction.normal() * (length / 2);
  Vec3 b = -a;

  auto sdf = [=](const Vec3& v)
  {
    Vec3 va  = v - a, ba = b - a;
    scalar s = clamp( va.dot(ba) / ba.norm2(), 0, 1);

    return ( va - ba * s ).norm() - radius;
  };

  Vec3 r{radius};
  Vec3 lb = -direction.abs() - r;

  return make_shared<ImplicitSurface>(sdf, gradient_from_sdf(sdf), 1.0, bounds::AABB{lb, -lb});
}

shared_ptr<Geometry> make_rounded_box(Vec3 size, scalar radius)
{
  Vec3 hw = size.abs() * 0.5;
  auto sdf = [=] (const Vec3& v)
  {
    return max(v.abs() - hw, Vec3::zero).norm() - radius;
  };

  auto ub = hw + Vec3{radius};
  return make_shared<ImplicitSurface>(sdf, gradient_from_sdf(sdf), 1.0, bounds::AABB{-ub, ub});
}
