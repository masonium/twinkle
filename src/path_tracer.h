#pragma once

#include "integrator.h"
#include "film.h"

#include <atomic>
#include <queue>
#include <mutex>
#include <memory>

using std::queue;
using std::mutex;
using std::shared_ptr;

struct Task
{
  Task() { }
  Task(Film::Rect rect_, uint spp) : rect(rect_), samples_per_pixel(spp) { }

  Film::Rect rect;
  uint samples_per_pixel;
};

typedef queue<Task> TaskQueue;

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

  void render(const Camera* cam, const Scene* scene, Film& film) override;
  spectrum trace_ray(const Scene* scene, const Ray& r,
                     shared_ptr<UniformSampler> sampler, int depth) const;

  long num_rays_traced() const { return rays_traced; }
  long num_primary_rays_traced() const { return primary_rays_traced; }

private:
  void render_thread(TaskQueue& queue, const Camera* cam, const Scene* scene,
                     shared_ptr<Film> film) const;

  mutable std::atomic_ullong rays_traced;
  mutable std::atomic_ullong primary_rays_traced;
  mutable mutex render_queue_mutex;

  Options opt;
};
