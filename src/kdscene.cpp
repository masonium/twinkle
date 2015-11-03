#include "kdscene.h"
#include <limits>

using std::move;
using std::numeric_limits;
using std::min;
using std::make_unique;

#if FEATURE_KD_SCENE == 1

KDScene::KDScene()
{
}

void KDScene::add(shared_ptr<const Shape> shape)
{
  shape_vector_.push_back(shape);
}

void KDScene::add(shared_ptr<const Light> light)
{
  lights_.push_back(light);
}

void KDScene::add(shared_ptr<const EnvironmentalLight> env_light_)
{
  this->env_light_ = env_light_;
}

void KDScene::prepare()
{
  vector<const Shape*> bounded_shapes;
  for (const auto& shape: shape_vector_)
  {
    if (shape->is_bounded())
      bounded_shapes.push_back(shape.get());
    else
      unbounded_shapes_.push_back(shape);
  }
  shape_tree_ = make_unique<kd::Tree<Shape>>(bounded_shapes, kd::TreeOptions());
}

const vector<shared_ptr<const Light>>& KDScene::lights() const
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
  return lights_[min(max_light_idx, decltype(max_light_idx)(r1 * lights_.size()))].get();
}

optional<Intersection> KDScene::intersect(const Ray& ray) const
{
  /*
   * In most scenes, you expect the infinite objects to behind the finite
   * objects. So, we test the finite objects first.
   */

  SubGeo best_geom = SUBGEO_NONE;
  Shape const* best_shape = nullptr;

  auto best_t = sfp_none;

  for (auto s: unbounded_shapes_)
  {
    SubGeo g = -1;
    auto t = s->intersect(ray, best_t, g);
    if (t < best_t)
    {
      best_t = t;
      best_shape = s.get();
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

#endif
