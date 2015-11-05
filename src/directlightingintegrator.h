#pragma once

#include "integrator.h"
#include "grid_tasks.h"
#include "scheduler.h"
#include "film.h"

class DirectLightingIntegrator : public RectIntegrator
{
public:
  struct Options
  {
    Options();

    double env_light_weight;
    unsigned int lighting_samples;
  };

  DirectLightingIntegrator() {}
  DirectLightingIntegrator(const Options& opt);

  void render_rect(const Camera& cam, const Scene& scene,
                   const Film::Rect& rect,
                   uint samples_per_pixel) const;

  virtual ~DirectLightingIntegrator() {}
private:

  spectrum trace_ray(const Scene& scene, const Ray& ray, Sampler& sampler) const;

  Options options;
};
