#pragma once

#include "integrator.h"
#include "scheduler.h"
#include "film.h"
#include "grid_tasks.h"

#include <atomic>
#include <queue>
#include <mutex>
#include <memory>

#ifdef FEATURE_PATH_TRACER

using std::queue;
using std::mutex;
using std::shared_ptr;

class PathTracerIntegrator : public RectIntegrator
{
public:
  struct Options
  {
    Options();

    bool russian_roulette;
    scalar rr_kill_prob;
    int min_rr_depth;
    int max_depth;
  };

  PathTracerIntegrator(const Options& opt);

  long num_rays_traced() const { return rays_traced; }
  long num_primary_rays_traced() const { return primary_rays_traced; }

  void render_rect(const Camera& cam, const Scene& scene,
                   const Film::Rect& rect,
                   uint samples_per_pixel) const;

  void pixel_samples(const Camera& cam, const Scene& scene,
                         uint x, uint y,
                         uint samples_per_pixel) const;

private:
  spectrum trace_ray(const Scene& scene, const Ray& r,
                     Sampler& sampler, int depth) const;

  mutable std::atomic_ullong rays_traced;
  mutable std::atomic_ullong primary_rays_traced;

  Options opt;
};

#endif
