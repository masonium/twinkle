#pragma once

#include <unordered_map>
#include <mutex>
#include "twinkle.h"
#include "integrator.h"
#include "sampler.h"

#ifdef FEATURE_DEBUG_TRACER

using ShapeColorMap = std::unordered_map<Shape const*, spectrum>;

class DebugIntegrator : public RayIntegrator
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
  };

  DebugIntegrator(const Options& opt_);

  spectrum trace_ray(const Scene& scene, const Ray& ray, Sampler& sampler) const override;

private:
  mutable std::mutex color_mutex;
  mutable ShapeColorMap color_map;
  std::unordered_map<Shape const*, int> shape_ids;
  Options opt;
};

#endif
