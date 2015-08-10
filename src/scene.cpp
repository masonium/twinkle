#include "scene.h"
#include <limits>

using std::move;
using std::numeric_limits;
using std::min;

Scene::Scene()
{
}

void Scene::add(shared_ptr<const Shape> shape)
{
  shapes_.push_back(shape);
}

void Scene::add(shared_ptr<const Light> light)
{
  lights_.push_back(light);
}
void Scene::add(shared_ptr<const EnvironmentalLight> env_light_)
{
  this->env_light_ = env_light_;
}

const vector<shared_ptr<const Light>>& Scene::lights() const
{
  return lights;
}


Light const* Scene::sample_light(scalar r1, scalar& light_prob) const
{
  if (lights_.size() == 0)
  {
    light_prob = 0.0;
    return nullptr;
  }

  light_prob = scalar{1.0} / lights_.size();

  auto max_light_idx = lights_.size() - 1;
  return lights_[min(max_light_idx, decltype(max_light_idx)(r1 * lights_.size()))].get();
}


Intersection Scene::intersect(const Ray& ray) const
{
  shared_ptr<const Shape> best_shape{nullptr};
  SubGeo best_geom;
  scalar best_t = numeric_limits<double>::max();

  for (auto s: shapes_)
  {
    SubGeo g = -1;
    scalar t = s->intersect(ray, best_t, g);
    if (t > 0 && t < best_t)
    {
      best_t = t;
      best_shape = s;
      best_geom = g;
    }
  }
  if (best_shape != nullptr)
    return Intersection(best_shape.get(), best_geom, ray, best_t);
  return Intersection(nullptr, 0, ray, -1);
}

spectrum Scene::environment_light_emission(const Vec3& dir) const
{
  return env_light_ ?  env_light_->emission(dir) : spectrum{0.0};
}
