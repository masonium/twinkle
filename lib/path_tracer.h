#pragma once

#include "integrator.h"

class PathTracerIntegrator : public Integrator
{
public:
  struct Options
  {
    Options()
      : russian_roulette(true), rr_kill_prob(0.1),
        max_depth(10), samples_per_pixel(16)
    {
      assert( 0.0 <= rr_kill_prob && rr_kill_prob <= 1.0 );
    }
  
    bool russian_roulette;
    scalar rr_kill_prob;
    int max_depth;
    uint samples_per_pixel;  
  };
  
  PathTracerIntegrator(const Options& opt);
  
  virtual void render(Camera* cam, Scene* scene, Film* film) override;

private:
  spectrum trace_ray(Scene* scene, const Ray& r, int depth);
  spectrum trace_shadow_ray(Scene* scene, PossibleEmissive const* em, const Ray& r);

  spectrum environmental_lighting(const Vec3& ray_dir) const;

  unique_ptr<UniformSampler> sampler;

  Options opt;
};
