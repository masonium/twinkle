#include "scene.h"
#include <limits>

using std::move;
using std::numeric_limits;

Scene::Scene()
{
}

void Scene::add_shape(Shape* shape)
{
  shapes.push_back(shape);
}

void Scene::add_light(Light* light)
{
  lights.push_back(light);
}


Intersection&& Scene::intersect(const Ray& ray) const
{
  Shape const* best_shape = nullptr;
  scalar best_t = numeric_limits<double>::max();

  for (auto s: shapes)
  {
    scalar t = s->intersect(ray);
    if (t > 0 && t < best_t)
    {
      best_t = t;
      best_shape = s;
    }
  }
  if (best_shape != nullptr)
    return move(Intersection(best_shape, ray, best_t));
  return move(Intersection(nullptr, ray, -1));
}
