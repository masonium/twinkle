#include "scene.h"
#include <limits>

using std::move;
using std::numeric_limits;
using std::min;

Scene::Scene()
{
}

void Scene::add(Shape* shape)
{
  shapes.push_back(shape);
}

void Scene::add(Light* light)
{
  lights.push_back(light);
}


Light const* Scene::sample_light(scalar r1, scalar& light_prob) const
{
  if (lights.size() == 0)
  {
    light_prob = 0.0;
    return nullptr;
  }

  light_prob = scalar{1.0} / lights.size();

  auto max_light_idx = lights.size() - 1;
  return lights[min(max_light_idx, decltype(max_light_idx)(r1 * lights.size()))];
}

Intersection Scene::intersect(const Ray& ray) const
{
  Shape const* best_shape = nullptr;
  Geometry const* best_geom = nullptr;
  scalar best_t = numeric_limits<double>::max();

  for (auto s: shapes)
  {
    Geometry const* g = nullptr;
    scalar t = s->intersect(ray, g);
    if (t > 0 && t < best_t)
    {
      best_t = t;
      best_shape = s;
      best_geom = g;
    }
  }
  if (best_shape != nullptr)
    return Intersection(best_shape, best_geom, ray, best_t);
  return Intersection(nullptr, nullptr, ray, -1);
}
