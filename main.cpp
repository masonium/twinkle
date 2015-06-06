#include <iostream>
#include <memory>

#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "bsdf.h"
#include "path_tracer.h"
#include "util.h"
#include "material.h"

using std::cerr;
using std::cout;
using std::make_shared;

void usage(char** args)
{
  cerr << args[0] << ": WIDTH HEIGHT SAMPLES-PER-PIXEL\n";
}

PerspectiveCamera single_sphere(Scene& scene, scalar aspect_ratio)
{
  scene.add( new Shape( make_shared<Sphere>(Vec3::zero, 1.0),
                        make_shared<RoughColorMaterial>(0, spectrum{0.2, 0.2, 1.0})));

  scene.add( new Shape( make_shared<Sphere>(Vec3::y_axis * -1000, 999),
                        make_shared<RoughColorMaterial>(0, spectrum{0.2, 0.2, 0.2})));

  Vec3 light_dir = Vec3::z_axis;
  scene.add( new DirectionalLight( light_dir, spectrum{1.0}));

  Vec3 light_pos = Vec3::z_axis * 6;
  scene.add( new DirectionalLight( light_pos, spectrum{1.0}));

  return PerspectiveCamera(Vec3(0, 0, 2), Vec3::zero, Vec3::y_axis, PI / 2.0,
                           aspect_ratio);
}

PerspectiveCamera default_scene(Scene& scene, scalar aspect_ratio)
{
  auto check = make_shared<GridTexture2D>(spectrum::one, spectrum::zero, 10.0, 0.1);

  // scene.add( new Shape( make_shared<Sphere>(Vec3{0.0, -1.0, 0.0}, 1.0),
  //                       make_shared<RoughMaterial>(0.0, check)));

  auto mat  = make_shared<GlassMaterial>();
  //auto mat  = make_shared<MirrorMaterial>();

  scene.add(new Shape(make_shared<Sphere>(Vec3{0.0, -1.0, 0.0}, 1.0), mat));
//  scene.add(new Shape(make_shared<ReversedGeometry>(make_shared<Sphere>(Vec3{0.0, -1.0, 0.0}, 0.75)), mat));

  scalar distance_from_center = 3.0;
  scalar sphere_radius = 1.0;

  const int num_sides = PI / atan(sphere_radius / (distance_from_center));
  for (int i = 0; i < num_sides; ++i)
  {
    const scalar angle = 2 * PI * i / num_sides;
    const scalar pr = 4.0;
    const Vec3 sp(pr*cos(angle), -2.0+sphere_radius, pr*sin(angle));
    auto sc = spectrum::from_hsv(i*360.0/num_sides, 1.0, 1.0);
    auto sc2 = spectrum::from_hsv(i*360.0/num_sides + 180, 0.75, 0.75);
    
    scene.add( new Shape( make_shared<Sphere>(sp, sphere_radius),
                          make_shared<RoughMaterial>(0.0, make_shared<GridTexture2D>(sc, sc2, 10.0, 0.2))));
  }

  scene.add( new Shape( make_shared<Sphere>(Vec3{0.0, -1000.0, 0.0}, 998.0),
                        make_shared<RoughColorMaterial>(0.0, spectrum{0.6})));

  const int num_lights = 3;
  for (int i = 0; i < num_lights; ++i)
  {
    const scalar angle = 2 * PI * i / num_lights + PI/12;
    const scalar pr = 4.0;
    scene.add( new PointLight{ Vec3(pr*cos(angle), 2.0, pr*sin(angle)), spectrum{1.0} });
  }

  auto cam_pos = Vec3{0, 2, 7.5}*0.8;

  return PerspectiveCamera(cam_pos, Vec3{0, -2.0, 0}, Vec3{0, 1, 0}, PI / 2.0,
                           aspect_ratio);
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

  cerr << "Rendered " << igr.num_primary_rays_traced() << " rays.\n";

  f.render_to_ppm(cout, make_shared<LinearToneMapper>());
  //f.render_to_ppm(cout, make_shared<RSSFToneMapper>());
  
  return 0;
}
