#pragma once

#include "integrator.h"

class DirectLightingIntegrator : public Integrator
{
public:
  struct Options
  {
    Options();
    
    unsigned int samples_per_pixel;
    unsigned int lighting_samples;
    double env_light_weight;
  };

  void render(const Camera* cam, const Scene* scene, Film& film)  override;

  virtual ~DirectLightingIntegrator() {}
private:
  spectrum trace_ray(const Ray& ray, const Scene* scene);

  Options options;
};
