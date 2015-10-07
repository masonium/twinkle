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
#include "textures/skytexture.h"

using std::cerr;
using std::endl;
using std::cout;
using std::make_shared;

void usage(char** args)
{
  cerr << args[0] << ": WIDTH HEIGHT SAMPLES-PER-PIXEL\n";
}

int main(int argc, char** args)
{
  if (argc < 5)
  {
    usage(args);
    exit(1);
  }
  const uint WIDTH = atoi(args[1]);
  const uint HEIGHT = atoi(args[2]);

  if (WIDTH == 0 || HEIGHT == 0)
  {
    usage(args);
    exit(1);
  }

  const uint per_pixel = atoi(args[3]);
  if (per_pixel == 0)
  {
    usage(args);
    exit(1);
  }

  shared_ptr<Scene> scene;
  if (atoi(args[4]))
    scene = make_shared<KDScene>();
  else
    scene = make_shared<BasicScene>();

  auto cam = showcase_scene(*scene, scalar(WIDTH)/scalar(HEIGHT), 0);

  auto bf = make_shared<BoxFilter>();
  Film f(WIDTH, HEIGHT, bf);

#define RENDER_ALGO 0

#if RENDER_ALGO == 0
  PathTracerIntegrator::Options opt;
  opt.samples_per_pixel = per_pixel;
  opt.num_threads = 1;
  opt.max_depth = 10;
  PathTracerIntegrator igr(opt);
#elif RENDER_ALGO == 1
  DirectLightingIntegrator::Options opt;
  opt.samples_per_pixel = 4;
  opt.lighting_samples = per_pixel / 4;
  opt.subdivision = 4;
  opt.num_threads = 0;
  DirectLightingIntegrator igr(opt);
#else
  DebugIntegrator::Options opt;
  opt.type = DebugIntegrator::DI_NORMAL;
  opt.samples_per_pixel = per_pixel;
  DebugIntegrator igr(opt);
#endif

  // cerr << "Rendering image at " << WIDTH << "x" << HEIGHT << " resolution, "
  //      << per_pixel << " samples per pixel\n";
  scene->prepare();
  igr.render(*cam, *scene, f);

  auto mapper = make_shared<LinearToneMapper>();
  // auto mapper = make_shared<ReinhardGlobal>();
  //auto mapper = make_shared<ReinhardLocal>(ReinhardLocal::Options{});
  //auto mapper = make_shared<CutoffToneMapper>();

  f.render_to_ppm(cout, *mapper);
  //f.render_to_twi(cout);
  //f.render_to_console(cout);

  return 0;
}
