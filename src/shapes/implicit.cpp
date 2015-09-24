#include "implicit.h"
#include <memory>

static const scalar MIN_STEP = 0.0001;

using std::make_shared;

ImplicitSurface::ImplicitSurface(ImplicitEvalFunc f_, ImplicitGradFunc g_,
                                 scalar lipschitz_const)
  : f(f_), g(g_), L(lipschitz_const), bbox(Vec3{-1.1}, Vec3{1.1})
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
    return scalar_fp{};

  if (max_t < t0 || max_t < MIN_STEP || t1 < MIN_STEP)
    return scalar_fp{};

  if (max_t.is())
    t1 = std::min(max_t.get(), t1);

  scalar t = MIN_STEP;

  scalar rdn = r.direction.norm();
  scalar dist = f(r.evaluate(t));

  if (fabs(dist) < MIN_STEP)
    return scalar_fp{t};

  do
  {
    scalar t_diff = std::max(dist, MIN_STEP) / (rdn * L);
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

  return scalar_fp{};
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
