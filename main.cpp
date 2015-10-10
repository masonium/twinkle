#include <string>
#include <map>
#include <iostream>
#include <memory>
#include <sstream>

#include "camera.h"
#include "integrator.h"
#include "dintegrator.h"
#include "scene.h"
#include "kdscene.h"
#include "bsdf.h"
#include "path_tracer.h"
#include "directlightingintegrator.h"
#include "util.h"
#include "model.h"
#include "material.h"
#include "scenes/basic_scenes.h"
#include "reinhard.h"
#include "cpp-optparse/OptionParser.h"
#include "util/timer.h"
#include "util/running_stats.h"

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

int main(int argc, char** args)
{
  optparse::OptionParser parser;

  map<string, DebugIntegrator::Type> debug_map({
      {"isect", DebugIntegrator::Type::DI_ISECT},
      {"object_id", DebugIntegrator::Type::DI_OBJECT_ID},
      {"normal", DebugIntegrator::Type::DI_NORMAL},
      {"depth", DebugIntegrator::Type::DI_DEPTH},
      {"specular", DebugIntegrator::Type::DI_SPECULAR},
      {"first_env", DebugIntegrator::Type::DI_FIRST_ENV},
      {"time_intersect", DebugIntegrator::Type::DI_TIME_INTERSECT}
    });

  vector<string> debug_map_keys;
  std::transform(debug_map.begin(), debug_map.end(), 
                 std::inserter(debug_map_keys, debug_map_keys.end()), [](const auto& e) { return e.first; });

  parser.add_option("-w", "--width").action("store").type("int").set_default(400);
  parser.add_option("-h", "--height").action("store").type("int").set_default(300);
  parser.add_option("-t", "--threads").action("store").type("int").set_default(0);
  parser.add_option("-s", "--samples").action("store").type("int").set_default(1);
  parser.add_option("-b", "--benchmark").action("store").type("int").set_default(1);
  parser.add_option("-o", "--output_image").action("store_true").type("bool");
  parser.add_option("-q", "--no_image").action("store_false").dest("output_image");
  parser.add_option("-d", "--debug_type").action("store").choices(debug_map_keys).set_default("normal");
  parser.add_option("-i", "--integrator").action("store").choices(std::vector<string>({"path", "direct", "debug"})).set_default("path");
  parser.add_option("-m", "--mapper").action("store").choices(std::vector<string>({"linear", "cutoff"})).set_default("linear");
  parser.add_option("--kd").action("store_const").dest("scene_container").set_const("kd");
  parser.add_option("--basic").action("store_const").dest("scene_container").set_const("basic");

  parser.set_defaults("output_image", "true");
  parser.set_defaults("scene_container", "kd");

  auto& options = parser.parse_args(argc, args);

  const uint WIDTH = options.get("width").as<int>();
  const uint HEIGHT = options.get("height").as<int>();

  assert(WIDTH > 0);
  assert(HEIGHT > 0);

  const uint per_pixel = options.get("samples").as<int>();
  assert(per_pixel > 0);

  shared_ptr<Scene> scene;
  if (options["scene_container"] == "kd")
    scene = make_shared<KDScene>();
  else
    scene = make_shared<BasicScene>();

  auto cam = showcase_scene(*scene, scalar(WIDTH)/scalar(HEIGHT), 0);

  auto bf = make_shared<BoxFilter>();
  Film f(WIDTH, HEIGHT, bf);


  unique_ptr<Integrator> igr;
  string igr_type = options["integrator"];

  if (igr_type == "path")
  {
    PathTracerIntegrator::Options opt;
    opt.samples_per_pixel = per_pixel;
    opt.num_threads = options.get("threads").as<int>();
    opt.max_depth = 10;
    igr = make_unique<PathTracerIntegrator>(opt);
  }
  else if (igr_type == "direct")
  {
    DirectLightingIntegrator::Options opt;
    opt.samples_per_pixel = 4;
    opt.lighting_samples = per_pixel / 4;
    opt.subdivision = 4;
    opt.num_threads = 0;
    igr = make_unique<DirectLightingIntegrator>(opt);
  }
  else // if (igr_type == "debug")
  {
    DebugIntegrator::Options opt;
    string dt = options.get("debug_type");
    opt.type = debug_map.at(dt);
    opt.samples_per_pixel = per_pixel;
    igr = make_unique<DebugIntegrator>(opt);
  }

  scene->prepare();

  scalar render_time = 0;

  auto benchmark_trials = options.get("benchmark").as<int>();
  bool write_image =  options.get("output_image").as<bool>();
  if (benchmark_trials > 1)
  {
    write_image = false;
    RunningStats stats;
    
    cerr << "Benchmarking with " << benchmark_trials << " trials" << endl;
    const int toss = 3;

    for (int i = 0; i < benchmark_trials + toss; i += 1)
    {
      Film f(WIDTH, HEIGHT, bf);
      Timer tm;
      igr->render(*cam, *scene, f);
      if (i >= toss)
          stats.update(tm.since());
    }

    cerr << "Render Time: " << format_duration(stats.mean()) << " +/- " 
         << format_duration(stats.stdev()) << endl;
  }
  else
  {
    Timer tm;
    igr->render(*cam, *scene, f);
    render_time = tm.since();
    cerr << "Render Time: " << format_duration(render_time) << endl;
  }

  //if (options.get("time").as<bool>())


  shared_ptr<ToneMapper> mapper;
  string map_type = options.get("mapper");
  if (map_type == "cutoff")
  {
    mapper = make_shared<CutoffToneMapper>();
  }
  else // if (options["mapper"] == "linear")
  {      
    mapper = make_shared<LinearToneMapper>();
  }

  if (write_image)
    f.render_to_ppm(cout, *mapper);

  return 0;
}
