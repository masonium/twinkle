#include "scene.h"
#include <limits>

using std::move;
using std::numeric_limits;
using std::min;

BasicScene::BasicScene()
{
}

void BasicScene::add(const Shape* shape)
{
  shapes_.push_back(shape);
}

void BasicScene::add(const Light* light)
{
  lights_.push_back(light);
  // const auto env = dynamic_cast<const EnvironmentLight*>(light);
  // if (env)
  // {
  //   env_lights_.push_back(env);
  // }
}

const vector<const Light*>& BasicScene::lights() const
{
  return lights_;
}


Light const* BasicScene::sample_light(scalar r1, scalar& light_prob) const
{
  if (lights_.size() == 0)
  {
    light_prob = 0.0;
    return nullptr;
  }

  light_prob = scalar{1.0} / lights_.size();

  auto max_light_idx = lights_.size() - 1;
  return lights_[min(max_light_idx, decltype(max_light_idx)(r1 * lights_.size()))];
}


optional<Intersection> BasicScene::intersect(const Ray& ray) const
{
  const Shape* best_shape = nullptr;
  SubGeo best_geom = 0;
  scalar_fp best_t = sfp_none;

  for (auto s: shapes_)
  {
    SubGeo g = -1;
    scalar_fp t = s->intersect(ray, best_t, g);
    if (t < best_t)
    {
      best_t = t;
      best_shape = s;
      best_geom = g;
    }
  }
  if (best_shape != nullptr)
    return Intersection(best_shape, best_geom, ray, best_t.get());
  return none_tag;
}

spectrum BasicScene::environment_light_emission(const Vec3& dir) const
{
  spectrum s{0.0};
  for (const auto& env: env_lights_)
    s += env->emission(dir);

  return s;
}
