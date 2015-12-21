#include "integrator.h"
#include "util.h"
#include <thread>
#include <map>
#include <chrono>

auto time_worker_seed(uint worker_id)
{
  auto time_count = std::chrono::system_clock::now().time_since_epoch().count();
  auto seed = std::hash<decltype(time_count)>()(time_count);
  hash_combine(seed, worker_id);
  return seed;
}

////////////////////////////////////////////////////////////////////////////////

void RayIntegrator::render_rect(const Camera& cam, const Scene& scene,
                                const Film::Rect& rect,
                                uint samples_per_pixel) const
{
  auto sampler = UniformSampler{};
  auto& film = get_thread_film();

  for (uint py = rect.y; py < rect.y + rect.height; ++py)
  {
    for (uint px = rect.x; px < rect.x + rect.width; ++px)
    {
      for (uint d = 0; d < samples_per_pixel; ++d)
      {
        PixelSample ps = cam.sample_pixel(film.width, film.height, px, py, sampler);

        spectrum s = trace_ray(scene, ps.ray, sampler);
        film.add_sample(ps, s);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

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

template <class T>
class GridRenderTask : public LocalTask
{
public:
  GridRenderTask(const T& owner_, const RenderInfo& ri_,
                 const Film::Rect& rect_, uint spp_)
    : owner(owner_), ri(ri_), rect(rect_), spp(spp_)
  {
  }


  void run(uint UNUSED(worker_id)) override
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

  using RT = GridRenderTask<RectIntegrator>;
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

////////////////////////////////////////////////////////////////////////////////

class PSSMLTRenderTask : public LocalTask
{
public:
  PSSMLTRenderTask(const RayIntegrator& igr,
                   const Camera& cam, const Scene& sc,
                   const PSSMLT::Options& opt, scalar ti,
                   uint64_t samples_)
    : integrator(igr), camera(cam), scene(sc), options(opt),
      total_intensity(ti), total_samples(samples_)
  {
  }

  void run(uint worker_id) override
  {
    PSSMLT pss(options, total_intensity);
    pss.seed(time_worker_seed(worker_id));

    auto sampler = pss.nth_sampler(0, 1);

    auto& film = get_thread_film();

    auto rect = film.rect();
    for (uint64_t i = 0; i < total_samples; ++i)
    {
      sampler.new_sample();

      auto ps = camera.sample_pixel(rect.width, rect.height, sampler);
      auto v = integrator.trace_ray(scene, ps.ray, sampler);
      auto s = pss.finish_sample(ps, v);

      film.add_sample(s.location, s.value, s.weight);
    }
  }

private:
  const RayIntegrator& integrator;
  const Camera& camera;
  const Scene& scene;
  PSSMLT::Options options;
  scalar total_intensity;
  uint64_t total_samples;
};

void pssmlt_render(const RayIntegrator& igr, const Camera& cam, const Scene& scene,
                   Film& film, Scheduler& scheduler, const PSSMLT::Options& opt,
                   uint total_spp)
{
  float fgrid_spp = pow(total_spp, 1.0/2.5);

  if (fgrid_spp < 4)
  {
    std::cerr << "Too few samples per pixel (" << fgrid_spp << ") for PSSSMLT. Running grid_render\n";
    grid_render(igr, cam, scene, film, scheduler, std::max(2u, film.width / 32),
                total_spp);
    return;
  }

  int grid_spp = fgrid_spp;
  std::cerr << "Running Grid Render with SPP = " << grid_spp << "\n";

  grid_render(igr, cam, scene, film, scheduler,
              std::max(2u, film.width / 32), grid_spp);

  scalar ti = film.average_intensity();

  uint64_t total_samples = film.width * film.height * (total_spp - grid_spp);

  auto cc = scheduler.concurrency();

  std::cerr << "Average intensity of " << ti << "\n";
  std::cerr << "Running PSSMLT Render with Avg. SPP = " << total_spp - grid_spp << "\n";

  for (auto i = 0u; i < cc; ++i)
    scheduler.add_task(make_shared<PSSMLTRenderTask>(igr, cam, scene, opt, ti,
                                                     total_samples / cc));

  scheduler.complete_pending();

  film.clear();
  merge_thread_films(film);

}
