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
  virtual void render(const Camera& cam, const Scene& scene, Film& film) = 0;

  virtual ~Integrator() { }
};

class RenderInfo
{
public:
  RenderInfo(const Camera& cam_, const Scene& scene_, const Film::Rect rect_)
    : camera(cam_), scene(scene_), rect(rect_)
  {
  }
  RenderInfo(const RenderInfo&) = delete;

  const Camera& camera;
  const Scene& scene;
  const Film::Rect rect;
};
