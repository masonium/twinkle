#pragma once

#include "camera.h"
#include "scene.h"

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
  
