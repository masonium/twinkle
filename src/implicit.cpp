#include "implicit.h"
#include <memory>

static const scalar MIN_STEP = 0.0001;
static const scalar SURFACE_EPSILON = 0.0001;

using std::make_shared;

ImplicitSurface::ImplicitSurface(ImplicitEvalFunc f_, ImplicitGradFunc g_,
                                 scalar lipschitz_const)
  : f(f_), g(g_), L(lipschitz_const), bbox(Vec3{-1.1}, Vec3{1.1})
{

}


Vec3 ImplicitSurface::normal(const Vec3& point) const
{
  return g(point).normal();
}

Vec3 ImplicitSurface::sample_shadow_ray_dir(const Intersection& isect,
                                            scalar r1,
                                            scalar r2) const
{
  return Vec3::zero;
}


scalar ImplicitSurface::intersect(const Ray& r, scalar max_t) const
{
  scalar t0, t1;
  if (!bbox.intersect(r, t0, t1))
    return -1;
  if (max_t < t0 || t1 < 0)
    return -1;

  t1 = min(max_t, t1);

  scalar t = 0;

  scalar rdn = r.direction.norm();
  scalar dist = f(r.evaluate(t));

  if (fabs(dist) < SURFACE_EPSILON)
    return t;

  do
  {
    scalar t_diff = max(dist, MIN_STEP) / (rdn * L);
    scalar new_t = t + t_diff;
    scalar new_dist = f(r.evaluate(new_t));
    if (fabs(new_dist) < SURFACE_EPSILON)
    {
      //return t + (dist - 0) / (dist - new_dist) * t_diff;
      return new_t;
    }

    t = new_t;
    dist = new_dist;
  } while (t < t1);

  return -1;
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
