#pragma once

#include "integrator.h"
#include "grid_tasks.h"
#include "scheduler.h"
#include "film.h"

class DirectLightingIntegrator : public RayIntegrator
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

  spectrum trace_ray(const Scene& scene, const Ray& ray, Sampler& sampler) const;

  virtual ~DirectLightingIntegrator() {}
private:



  Options options;
};
