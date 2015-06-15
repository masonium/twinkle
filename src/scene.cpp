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
  shapes.push_back(shape);
}

void Scene::add(shared_ptr<const Light> light)
{
  lights.push_back(light);
}
void Scene::add(shared_ptr<const EnvironmentalLight> env_light)
{
  env_lights.push_back(env_light);
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
  return lights[min(max_light_idx, decltype(max_light_idx)(r1 * lights.size()))].get();
}

Intersection Scene::intersect(const Ray& ray) const
{
  shared_ptr<const Shape> best_shape;
  Geometry const* best_geom = nullptr;
  scalar best_t = numeric_limits<double>::max();

  for (auto s: shapes)
  {
    Geometry const* g = nullptr;
    scalar t = s->intersect(ray, best_t, g);
    if (t > 0)
    {
      best_t = t;
      best_shape = s;
      best_geom = g;
    }
  }
  if (best_shape != nullptr)
    return Intersection(best_shape.get(), best_geom, ray, best_t);
  return Intersection(nullptr, nullptr, ray, -1);
}

spectrum Scene::environment_light_emission(const Vec3& dir) const
{
  return accumulate(env_lights.begin(), env_lights.end(),
                    spectrum::zero, [&dir](spectrum ret, auto env_light)
                    {
                      return ret + env_light->emission(dir);
                    });
}
