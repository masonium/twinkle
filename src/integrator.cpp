#include "integrator.h"

template <class T>
class RenderTask : public LocalTask
{
public:
  RenderTask(const T& owner_, const RenderInfo& ri_,
             const Film::Rect& rect_, uint spp_)
    : owner(owner_), ri(ri_), rect(rect_), spp(spp_)
  {
  }


  void run(uint worker_id) override
  {
    owner.render_rect(ri.camera, ri.scene, rect, spp);
  }

private:
  const T& owner;
  const RenderInfo& ri;
  Film::Rect rect;
  uint spp;
};

void grid_render(const RectIntegrator& renderer, const Camera& cam, const Scene& scene,
                 Film& film, Scheduler& scheduler, uint subdiv, uint total_spp)
{
  using std::for_each;
  using std::transform;

  RenderInfo ri{cam, scene, film.rect()};

  grid_subtask_options opt;
  opt.grid_subdivision = subdiv;
  auto subrects = subtasks_from_grid(film.width, film.height, opt);

  using RT = RenderTask<RectIntegrator>;
  vector<shared_ptr<RT>> render_tasks;
  render_tasks.resize(subrects.size());

  transform(subrects.begin(), subrects.end(), render_tasks.begin(),
            [&](auto& rect) { return make_shared<RT>(renderer, ri,
                                                     rect, total_spp); });

  for_each(render_tasks.begin(), render_tasks.end(),
           [&](auto& task) { scheduler.add_task(task); });

  scheduler.complete_pending();

  merge_thread_films(film);
}
