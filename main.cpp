#include <iostream>

#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "bsdf.h"
#include "path_tracer.h"
#include "util.h"

using std::cerr;
using std::cout;

void smallpt_scene(Scene& scene)
{
  BRDF* diffuse = new Diffuse{1.0};
  BRDF* light = new EmissiveBRDF{spectrum{12.0}};
  scene.add(new Shape(new Sphere{ Vec3{1e5+1, 40.8, 81.6}, 1e5 },
                      diffuse,
                      new SolidColor(spectrum{0.75,0.25,0.25})));
  scene.add(new Shape(new Sphere{ Vec3{-1e5+99, 40.8, 81.6}, 1e5 },
                      diffuse,
                      new SolidColor(spectrum{0.25, 0.25, 0.75})));
  scene.add(new Shape(new Sphere{Vec3{50, 40.8, 1e5}, 1e5},
                      diffuse,
                      new SolidColor(spectrum{0.75})));
  scene.add(new Shape(new Sphere{Vec3{50, 1e5, 81.6}, 1e5},
                      diffuse,
                      new SolidColor(spectrum{0.75})));
  scene.add(new Shape(new Sphere{Vec3{50, -1e5+81.6, 81.6}, 1e5},
                      diffuse,
                      new SolidColor(spectrum{0.75})));

  scene.add(new Shape(new Sphere{Vec3{50,681.6-.27,81.6}, 600},
                      light,
                      new SolidColor(spectrum{1.0})));
}

void usage(char** args)
{
  cerr << args[0] << ": WIDTH HEIGHT SAMPLES-PER-PIXEL\n";
}

PerspectiveCamera default_scene(Scene& scene, scalar aspect_ratio)
{
  BRDF* b = new Diffuse{1.0};

  BRDF* mirror = new PerfectMirrorBRDF{};

  //smallpt_scene(scene);

  scene.add( new Shape( new Sphere{ Vec3{0.0, -1.0, 0.0}, 2.0},
                              mirror,
                              new SolidColor(spectrum{1.0})) );

  // scene.add( new Shape( new Sphere{ Vec3{2.0, -1.0, 0.0}, 1.0},
  //                             b,
  //                             new SolidColor(spectrum::from_hsv(0.0, 1.0, 0.8))) );

  scalar distance_from_center = 3.0;
  scalar sphere_radius = 0.75;

  const int num_sides = PI / atan(sphere_radius / (distance_from_center));
  for (int i = 0; i < num_sides; ++i)
  {
    const scalar angle = 2 * PI * i / num_sides;
    const scalar pr = 4.0;
    scene.add( new Shape( new Sphere{ Vec3(pr*cos(angle), -2.0+sphere_radius, pr*sin(angle)), sphere_radius},
                                b,
                                new SolidColor(spectrum::from_hsv(i*360/num_sides, 1.0, 1.0))));
  }

  scene.add( new Shape( new Sphere{ Vec3{0.0, -1000.0, 0.0}, 998.0},
                              b,
                              new SolidColor(spectrum{0.6})));

  const int num_lights = 3;
  for (int i = 0; i < num_lights; ++i)
  {
    const scalar angle = 2 * PI * i / num_lights + PI/12;
    const scalar pr = 4.0;
    scene.add( new PointLight{ Vec3(pr*cos(angle), 2.0, pr*sin(angle)), spectrum{5.0} });
  }

  auto cam_pos = Vec3{0, 2, 7.5}*0.8;

  return PerspectiveCamera(cam_pos, Vec3{0, -2.0, 0}, Vec3{0, 1, 0}, PI / 2.0,
                           aspect_ratio);
}

PerspectiveCamera dof_scene(Scene& scene, scalar ar)
{
  BRDF* mirr = new PerfectMirrorBRDF();
  BRDF* diff = new Diffuse(1.0);

  scene.add( new Shape( new Sphere( Vec3{0, 0, -2}, 1.0 ),
                        mirr,
                        new SolidColor(spectrum::one)));

  scene.add( new Shape(new Sphere( Vec3{0, 0, 2}, 1.0 ),
                       diff,
                       new SolidColor(spectrum{1.0, 0.2, 0.3})));

  scene.add( new Shape(new Sphere( Vec3{0.5, 0, -0.5}, 0.2),
                       diff,
                       new SolidColor(spectrum{0.3, 0.1, 1.0})));

  scene.add( new DirectionalLight( Vec3(-1, 1, -1), spectrum{5.0}));

  return PerspectiveCamera(Vec3::zero, -Vec3::z_axis, Vec3::y_axis,
                           PI/2, ar, 0.02, 1.0);
}

int main(int argc, char** args)
{
  if (argc < 4)
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

  Scene scene;
  PerspectiveCamera cam = default_scene(scene, scalar(WIDTH)/scalar(HEIGHT));

  Film f(WIDTH, HEIGHT, new BoxFilter);

  PathTracerIntegrator::Options opt;

  const uint per_pixel = atoi(args[3]);
  if (per_pixel == 0)
  {
    usage(args);
    exit(1);
  }

  opt.samples_per_pixel = per_pixel;
  if (argc >= 5)
    opt.num_threads = atoi(args[4]);
  else
    opt.num_threads = 1;
  
  PathTracerIntegrator igr(opt);

  cerr << "Rendering image at " << WIDTH << "x" << HEIGHT << " resolution, "
       << per_pixel << " samples per pixel\n";

  igr.render(&cam, &scene, f);

  cerr << "Rendered " << igr.num_primary_rays_traced() << " rays.";
  
  f.render_to_ppm(cout, new LinearToneMapper);

  return 0;
}
