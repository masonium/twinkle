#pragma once

#include <random>
#include <memory>
#include <algorithm>
#include "camera.h"
#include "scene.h"
#include "film.h"
#include "sampler.h"
#include "pss_sampler.h"
#include "grid_tasks.h"
#include "scheduler.h"
#include "util.h"

using std::unique_ptr;
using std::make_shared;

/**
 * A rect integrator will render on a region a film, with samples_per_pixel as a
 * (potentailly amortized) utilization amount.
 */
class RectIntegrator
{
public:
  virtual void render_rect(const Camera& cam, const Scene& scene,
                           const Film::Rect& rect,
                           uint samples_per_pixel) const = 0;
};

/**
 * A ray integrator is any evaluator that maps rays to radiance along the
 * ray.
 */
class RayIntegrator : public RectIntegrator
{
public:
  void render_rect(const Camera& cam, const Scene& scene,
                   const Film::Rect& rect,
                   uint samples_per_pixel) const override;

  virtual spectrum trace_ray(const Scene& scene, const Ray& ray,
                             Sampler& sampler) const = 0;
};


void grid_render(const RayIntegrator& renderer, const Camera& cam, const Scene& scene,
                 Film& film, Scheduler& scheduler, uint total_spp);

struct PMCGridOptions
{
  uint initial_spp;
  uint follow_spp;
  uint num_iterations;
};

void pmc_render(const RayIntegrator&, const Camera& cam, const Scene& scene,
                Film& film,  Scheduler& scheduler, const PMCGridOptions& opt);

void pssmlt_render(const RayIntegrator&, const Camera& cam, const Scene& scene,
                   Film& film, Scheduler& scheduler, const PSSMLT::Options& opt,
                   uint total_spp);
