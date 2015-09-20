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

class RectIntegrator : public Integrator
{
public:
  virtual void render_rect(const Camera& cam, const Scene& scene,
                           Film& film, const Film::Rect& rect,
                           uint samples_per_pixel) const = 0;
};

template <class T>
class RenderTask : public LocalTask
{
public:
  RenderTask(const T& owner_, const RenderInfo& ri_,
             vector<Film>& films_, const Film::Rect& rect_,
             uint spp_)
    : owner(owner_), ri(ri_), films(films_), rect(rect_), spp(spp_)
  {
  }


  void run(uint worker_id) override
  {
    owner.render_rect(ri.camera, ri.scene, films[worker_id], rect, spp);
  }

private:
  const T& owner;
  const RenderInfo& ri;
  vector<Film>& films;
  Film::Rect rect;
  uint spp;
};

template <class T>
void grid_render(const T& renderer, const Camera& cam, const Scene& scene, Film& film,
                 uint num_threads, uint subdiv, uint total_spp)
{
  using std::for_each;
  using std::transform;

  num_threads = num_threads || num_system_procs();

  LocalThreadScheduler lts{num_threads};

  vector<Film> films;
  for (auto i = 0u; i < num_threads; ++i)
    films.emplace_back(film.width, film.height, film.filter);


  RenderInfo ri{cam, scene, film.rect()};

  grid_subtask_options opt;
  opt.grid_subdivision = subdiv;
  auto subrects = subtasks_from_grid(film.width, film.height, opt);
  using RT = RenderTask<T>;
  vector<shared_ptr<RT>> render_tasks;
  render_tasks.resize(subrects.size());

  transform(subrects.begin(), subrects.end(), render_tasks.begin(),
            [&](auto& rect) { return make_shared<RT>(renderer, ri, std::ref(films),
                                                     rect, total_spp); });

  for_each(render_tasks.begin(), render_tasks.end(),
           [&](auto& task) { lts.add_task(task); });

  lts.complete_pending();

  for (const auto& f: films)
    film.merge(f);

}
