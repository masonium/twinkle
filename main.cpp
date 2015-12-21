#include <string>
#include <map>
#include <iostream>
#include <memory>
#include <sstream>

#include "directlightingintegrator.h"
#include "debugintegrator.h"
#include "path_tracer.h"

#include "camera.h"

#include "scene.h"
#include "kdscene.h"

#include "scenes/basic_scenes.h"
#include "reinhard.h"

#include "util/timer.h"
#include "util/running_stats.h"
#include "thread_state.h"
#include "pss_sampler.h"

#include "cpp-optparse/OptionParser.h"

using std::cerr;
using std::endl;
using std::cout;
using std::map;
using std::vector;
using std::make_unique;
using std::make_shared;

void usage(char** args)
{
  cerr << args[0] << ": WIDTH HEIGHT SAMPLES-PER-PIXEL\n";
}

const  map<string, DebugIntegrator::Type> DEBUG_TYPE_MAP({
    {"isect", DebugIntegrator::Type::DI_ISECT},
    {"object_id", DebugIntegrator::Type::DI_OBJECT_ID},
    {"normal", DebugIntegrator::Type::DI_NORMAL},
    {"depth", DebugIntegrator::Type::DI_DEPTH},
    {"specular", DebugIntegrator::Type::DI_SPECULAR},
    {"specular_p", DebugIntegrator::Type::DI_SPECULAR_P},
    {"first_env", DebugIntegrator::Type::DI_FIRST_ENV},
    {"time_intersect", DebugIntegrator::Type::DI_TIME_INTERSECT}
  });


auto parse_args(int argc, char**args)
{
  optparse::OptionParser parser;


  vector<string> debug_map_keys;
  std::transform(DEBUG_TYPE_MAP.begin(), DEBUG_TYPE_MAP.end(), 
                 std::inserter(debug_map_keys, debug_map_keys.end()), [](const auto& e) { return e.first; });

  parser.add_option("-w", "--width").action("store").type("int").set_default(400);
  parser.add_option("-h", "--height").action("store").type("int").set_default(300);
  parser.add_option("-t", "--threads").action("store").type("int").set_default(1);
  parser.add_option("-s", "--samples").action("store").type("int").set_default(5);
  parser.add_option("-b", "--benchmark").action("store").type("int").set_default(1);
  parser.add_option("-o", "--output_image").action("store_true").type("bool");
  parser.add_option("-q", "--no_image").action("store_false").dest("output_image");
  parser.add_option("-d", "--debug_type").action("store").choices(debug_map_keys).set_default("normal");
  parser.add_option("-i", "--integrator").action("store").choices(std::vector<string>({"path", "direct", "debug"})).set_default("path");
  parser.add_option("--pss").action("store_true").type("bool").dest("pssmlt").set_default(false);
  parser.add_option("-m", "--mapper").action("store").choices(std::vector<string>({"linear", "cutoff", "rh_global"})).set_default("linear");
  parser.add_option("--kd").action("store_const").dest("scene_container").set_const("kd");
  parser.add_option("--basic").action("store_const").dest("scene_container").set_const("basic");

  parser.add_option("-f", "--scene").action("store").dest("scene").set_default("assets/scripts/scene1.lua");

  parser.add_option("--weights").action("store_true").set_default(false);
  parser.add_option("--pv").action("store_true").set_default(false);

  parser.set_defaults("output_image", "true");
  parser.set_defaults("scene_container", "kd");

  return parser.parse_args(argc, args);
}

int main(int argc, char** args)
{
  auto options = parse_args(argc, args);

  const uint WIDTH = options.get("width").as<int>();
  const uint HEIGHT = options.get("height").as<int>();

  assert(WIDTH > 0);
  assert(HEIGHT > 0);

  uint per_pixel = options.get("samples").as<int>();
  assert(per_pixel > 0);

  shared_ptr<Scene> scene;
  if (options["scene_container"] == "kd")
    scene = make_shared<KDScene>();
  else
    scene = make_shared<BasicScene>();

  string scene_filename = options.get("scene").as<string>();

  Camera* cam;
  {
    Timer tm;
    cam = lua_scene(*scene, scene_filename);
    cerr << "Loaded scene in " << format_duration(tm.since()) << "\n";
  }

  Film f(WIDTH, HEIGHT);

  register_thread_state_manager(f, scene_filename);
  auto num_threads = options.get("threads").as<int>();
  if (num_threads == 0)
    num_threads = num_system_procs();
  cerr << "using " << num_threads << " threads.\n";

  unique_ptr<RayIntegrator> igr;
  string igr_type = options["integrator"];

  if (igr_type == "path")
  {
    PathTracerIntegrator::Options opt;
    opt.max_depth = 10;
    igr = make_unique<PathTracerIntegrator>(opt);
  }
  else if (igr_type == "direct")
  {
    DirectLightingIntegrator::Options opt;
    opt.lighting_samples = per_pixel / 4;
    per_pixel = 4;
    igr = make_unique<DirectLightingIntegrator>(opt);
  }
  else // if (igr_type == "debug")
  {
    DebugIntegrator::Options opt;
    string dt = options.get("debug_type");
    opt.type = DEBUG_TYPE_MAP.at(dt);
    igr = make_unique<DebugIntegrator>(opt);
  }

  {
    Timer tm;
    scene->prepare();
    cerr << "Prepared scene in " << format_duration(tm.since()) << endl;
  }

  scalar render_time = 0;

  auto benchmark_trials = options.get("benchmark").as<int>();
  bool write_image =  options.get("output_image").as<bool>();

  {
    auto scheduler = make_scheduler(num_threads);

    if (benchmark_trials > 1)
    {
      write_image = false;
      RunningStats stats;
    
      cerr << "Benchmarking with " << benchmark_trials << " trials" << endl;
      const int toss = 3;

      for (int i = 0; i < benchmark_trials + toss; i += 1)
      {
        Film f(WIDTH, HEIGHT);
        Timer tm;
        grid_render(*igr, *cam, *scene, f, *scheduler, 4, per_pixel);
        if (i >= toss)
          stats.update(tm.since());
      }

      cerr << "Render Time: " << format_duration(stats.mean()) << " +/- " 
           << format_duration(stats.stdev()) << endl;
    }
    else
    {
      Timer tm;
      PSSMLT::Options opt;
      opt.large_step_prob = 0.25;

      bool use_pssmlt = options.get("pssmlt").as<bool>();
      if (use_pssmlt)
        pssmlt_render(*igr, *cam, *scene, f, *scheduler, opt, per_pixel);
      else
        grid_render(*igr, *cam, *scene, f, *scheduler, 4, per_pixel);

      render_time = tm.since();
      cerr << "Render Time: " << format_duration(render_time) << endl;
    }
  }

  if (options.get("weights").as<bool>())
  {
    f = f.as_weights();
  }
  else if (options.get("pv").as<bool>())
  {
    f = f.as_pv();
  }


  shared_ptr<ToneMapper> mapper;
  string map_type = options.get("mapper");
  if (map_type == "cutoff")
  {
    mapper = make_shared<CutoffToneMapper>();
  }
  else if (map_type == "linear")
  {      
    mapper = make_shared<LinearToneMapper>();
  }
  else if (map_type == "rh_global")
  {
    mapper = make_shared<ReinhardGlobal>();
  }

  if (write_image)
    f.render_to_ppm(cout, *mapper);

  return 0;
}
