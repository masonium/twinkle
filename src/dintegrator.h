#pragma once

#include <unordered_map>
#include "integrator.h"
#include "sampler.h"

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
    DI_SPECULAR,
    DI_FIRST_ENV,
    DI_TIME_INTERSECT
  };

  struct Options
  {
    Type type;
    int samples_per_pixel;
  };

  DebugIntegrator(const Options& opt_);

  void render(const Camera& cam, const Scene& scene, Scheduler& scheduler, Film& film) override;

  void render_rect(const Camera& cam, const Scene& scene,
                   const Film::Rect& rect,
                   uint samples_per_pixel) const override;

private:
  spectrum trace_ray(const Ray& ray, const Scene& scene,
                     ShapeColorMap& scm, Sampler& sampler) const;

  std::unordered_map<Shape const*, int> shape_ids;
  Options opt;
};

#endif
