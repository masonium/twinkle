#pragma once

#include <random>
#include <memory>
#include "camera.h"
#include "scene.h"
#include "film.h"
#include "sampler.h"

using std::unique_ptr;

class Integrator
{
public:
  virtual void render(Camera* cam, Scene* scene, Film* film) = 0;
};


class BWIntegrator : public Integrator
{
public:
  virtual void render(Camera* cam, Scene* scene, Film* film) override;
};
  
class PathTracerIntegrator : public Integrator
{
public:
  PathTracerIntegrator();
  
  virtual void render(Camera* cam, Scene* scene, Film* film) override;

private:
  spectrum trace_ray(Scene* scene, const Ray& r, int depth);
  spectrum trace_shadow_ray(Scene* scene, PossibleEmissive const* em, const Ray& r);

  unique_ptr<UniformSampler> sampler;  
  uint samples_per_pixel;
  bool russian_roulette;
  scalar rr_kill_prob;
  int max_depth;
};
