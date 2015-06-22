#pragma once

#include <unordered_map>
#include "integrator.h"

class DebugIntegrator : public Integrator
{
public:
  enum Type
  {
    DI_ISECT,
    DI_OBJECT_ID,
    DI_NORMAL,
    DI_SPECULAR
  };

  DebugIntegrator(Type type);

  void render(const Camera* cam, const Scene* scene, Film& film) override;

private:
  spectrum trace_ray(const Ray& ray, const Scene* scene);

  std::unordered_map<Shape const*, int> shape_ids;
  Type type;
};
