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

  DirectLightingIntegrator() {}
  DirectLightingIntegrator(const Options& opt);

  void render(const Camera* cam, const Scene* scene, Film& film)  override;

  virtual ~DirectLightingIntegrator() {}
private:
  spectrum trace_ray(const Ray& ray, const Scene* scene, shared_ptr<Sampler> sampler) const;

  Options options;
};
