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

class PathTracerIntegrator : public RayIntegrator
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

  spectrum trace_ray(const Scene& scene, const Ray& r,
                     Sampler& sampler) const override;

private:
  spectrum _trace_ray(const Scene& scene, const Ray& r,
                     Sampler& sampler, int depth) const;

  mutable std::atomic_ullong rays_traced;
  mutable std::atomic_ullong primary_rays_traced;

  Options opt;
};

#endif
