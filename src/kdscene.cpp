#include <limits>
#include "util.h"
#include "kdscene.h"

using std::move;
using std::numeric_limits;
using std::min;
using std::make_unique;

KDScene::KDScene() : env_light_(nullptr)
{
}

void KDScene::add(const Shape* shape)
{
  shape_vector_.push_back(shape);
}

void KDScene::add(const Light* light)
{
  lights_.push_back(light);
  const auto env = dynamic_cast<const EnvironmentLight*>(light);
  if (env)
    env_light_ = env;
}

void KDScene::prepare()
{
  vector<const Shape*> bounded_shapes;
  for (const auto& shape: shape_vector_)
  {
    if (shape->is_bounded())
      bounded_shapes.push_back(shape);
    else
      unbounded_shapes_.push_back(shape);
  }
  shape_tree_ = make_unique<kd::Tree<Shape>>(bounded_shapes, kd::TreeOptions());
}

const vector<const Light*>& KDScene::lights() const
{
  return lights_;
}


Light const* KDScene::sample_light(scalar r1, scalar& light_prob) const
{
  if (lights_.size() == 0)
  {
    light_prob = 0.0;
    return nullptr;
  }

  light_prob = 1.0 / lights_.size();

  auto max_light_idx = lights_.size() - 1;
  return lights_[min(max_light_idx, decltype(max_light_idx)(r1 * lights_.size()))];
}

EmissionSample KDScene::sample_emission(Sampler& sampler) const
{
  scalar lp = 0;

  // sample the light, then sample an emission from the light.
  auto light = sample_light(sampler.sample_1d(), lp);
  if (unlikely(lp == 0))
    return EmissionSample(light, Ray(Vec3::zero, Vec3::z_axis), 0.0);

  auto es = light->sample_emission(*this, sampler);
  es.light_prob = lp;

  return es;
}

optional<Intersection> KDScene::intersect(const Ray& ray, scalar_fp max_t) const
{
  /*
   * In most scenes, you expect the infinite objects to behind the finite
   * objects. So, we test the finite objects first.
   */

  SubGeo best_geom = SUBGEO_NONE;
  Shape const* best_shape = nullptr;

  auto best_t = max_t;

  for (auto s: unbounded_shapes_)
  {
    SubGeo g = -1;
    auto t = s->intersect(ray, best_t, g);
    if (t < best_t)
    {
      best_t = t;
      best_shape = s;
      best_geom = g;
    }
  }

  {
    SubGeo geom = SUBGEO_NONE;
    Shape const* shape = nullptr;

    auto t = shape_tree_->intersect(ray, best_t, shape, geom);
    if (t < best_t)
    {
      best_t = t;
      best_shape = shape;
      best_geom = geom;
    }

  }
  if (best_t.is())
    return Intersection(best_shape, best_geom, ray, best_t.get());

  return none_tag;
}

spectrum KDScene::environment_light_emission(const Vec3& dir) const
{
  return env_light_ ? env_light_->emission(dir) : spectrum::zero;
}
