#pragma once

#include <unordered_map>
#include "integrator.h"

#define FEATURE_DEBUG_TRACER 1
#ifdef FEATURE_DEBUG_TRACER

using ShapeColorMap = std::unordered_map<Shape const*, spectrum>;

class DebugIntegrator : public RectIntegrator
{
public:
  enum Type
  {
    DI_ISECT,
    DI_OBJECT_ID,
    DI_NORMAL,
    DI_DEPTH,
    DI_SPECULAR
  };

  DebugIntegrator(Type type);

  void render(const Camera& cam, const Scene& scene, Film& film) override;

  void render_rect(const Camera& cam, const Scene& scene,
                   Film& film, const Film::Rect& rect,
                   uint samples_per_pixel) const override;

private:
  spectrum trace_ray(const Ray& ray, const Scene& scene, ShapeColorMap& scm) const;

  std::unordered_map<Shape const*, int> shape_ids;
  Type type;
};

#endif
