#pragma once

#include <random>
#include <memory>
#include <algorithm>
#include "camera.h"
#include "scene.h"
#include "film.h"
#include "sampler.h"
#include "grid_tasks.h"
#include "scheduler.h"
#include "util.h"

using std::unique_ptr;
using std::make_shared;

class RenderInfo
{
public:
  RenderInfo(const Camera& cam_, const Scene& scene_, const Film::Rect& rect_)
    : camera(cam_), scene(scene_), rect(rect_)
  {
  }
  RenderInfo(const RenderInfo&) = delete;

  const Camera& camera;
  const Scene& scene;
  const Film::Rect rect;
};

class RectIntegrator
{
public:
  virtual void render_rect(const Camera& cam, const Scene& scene,
                           const Film::Rect& rect,
                           uint samples_per_pixel) const = 0;
};

void grid_render(const RectIntegrator& renderer, const Camera& cam, const Scene& scene, Film& film,
                 Scheduler& scheduler, uint subdiv, uint total_spp);
