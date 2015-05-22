#pragma once

#include <random>
#include "camera.h"
#include "scene.h"
#include "film.h"

using std::default_random_engine;
using std::uniform_real_distribution;

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
  spectrum trace_ray(Scene* scene, const Ray& r);
  spectrum trace_shadow_ray(Scene* scene, PossibleEmissive const* em, const Ray& r);
  
  std::default_random_engine reng;
  std::uniform_real_distribution<scalar> randr;
  
  
  uint samples_per_pixel;
  bool russian_roulette;
  uint max_depth;
};
