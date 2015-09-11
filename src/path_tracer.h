#pragma once

#include "integrator.h"
#include "scheduler.h"
#include "film.h"
#include "grid_tasks.h"

#include <atomic>
#include <queue>
#include <mutex>
#include <memory>

using std::queue;
using std::mutex;
using std::shared_ptr;

class PathTracerIntegrator : public Integrator
{
public:
  struct Options
  {
    Options();

    bool russian_roulette;
    scalar rr_kill_prob;
    int min_rr_depth;
    int max_depth;
    uint samples_per_pixel;
    uint num_threads;
  };

  PathTracerIntegrator(const Options& opt);

  void render(const Camera& cam, const Scene& scene, Film& film) override;
  spectrum trace_ray(const Scene& scene, const Ray& r,
                     Sampler& sampler, int depth) const;

  long num_rays_traced() const { return rays_traced; }
  long num_primary_rays_traced() const { return primary_rays_traced; }

private:
  class RenderTask : public LocalTask
  {
  public:
    RenderTask(const PathTracerIntegrator* pit, const Camera& cam_, const Scene& scene_,
               const Film::Rect& rect_, uint spp_, vector<Film>& films_);


  RenderTask(const RenderTask&) = delete;
  RenderTask(RenderTask&&) = delete;

  void run(uint id) override;

  ~RenderTask() { }

private:
  const PathTracerIntegrator* owner;
  const Camera& cam;
  const Scene& scene;
  const Film::Rect& rect;
  uint spp;
  vector<Film>& films;
};

void render_rect(const Camera& cam, const Scene& scene,
                 const Film::Rect& rect, uint samples_per_pixel,
                 Film& film) const;

mutable std::atomic_ullong rays_traced;
mutable std::atomic_ullong primary_rays_traced;

Options opt;
};
