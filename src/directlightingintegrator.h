#pragma once

#include "integrator.h"
#include "grid_tasks.h"
#include "scheduler.h"
#include "film.h"

class DirectLightingIntegrator : public Integrator
{
public:
  struct Options
  {
    Options();

    double env_light_weight;
    unsigned int samples_per_pixel;
    unsigned int lighting_samples;
    uint num_threads;
    uint subdivision;
  };

  DirectLightingIntegrator() {}
  DirectLightingIntegrator(const Options& opt);

  void render(const Camera& cam, const Scene& scene, Film& film)  override;

  virtual ~DirectLightingIntegrator() {}

private:
  class RenderTask : public LocalTask
  {
  public:
    RenderTask(const DirectLightingIntegrator& dli, const RenderInfo&,
               vector<Film>&, const Film::Rect& rect_,
               uint spp);

    void run(uint id) override;

  private:
    const DirectLightingIntegrator& owner;
    const RenderInfo& ri;
    vector<Film>& films;
    Film::Rect rect;
    uint spp;
  };

  void render_rect(const Camera& cam, const Scene& scene,
                   Film& film, const Film::Rect& rect,
                   uint samples_per_pixel) const;


  spectrum trace_ray(const Scene& scene, const Ray& ray, Sampler& sampler) const;

  Options options;
};
