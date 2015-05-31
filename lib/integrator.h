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
  virtual void render(const Camera* cam, const Scene* scene, Film& film) = 0;
};


class BWIntegrator : public Integrator
{
public:
  virtual void render(const Camera* cam, const Scene* scene, Film& film) override;
};


