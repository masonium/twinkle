#include <iostream>
#include <memory>

#include "camera.h"
#include "integrator.h"
#include "scene.h"
#include "sphere.h"
#include "plane.h"
#include "bsdf.h"
#include "path_tracer.h"
#include "util.h"
#include "model.h"
#include "mesh.h"
#include "material.h"

using std::cerr;
using std::endl;
using std::cout;
using std::make_shared;

void usage(char** args)
{
  cerr << args[0] << ": WIDTH HEIGHT SAMPLES-PER-PIXEL\n";
}

PerspectiveCamera single_sphere(Scene& scene, scalar aspect_ratio)
{
  scene.add( make_shared<Shape>( make_shared<Sphere>(Vec3::zero, 1.0),
			make_shared<RoughColorMaterial>(0, spectrum{0.2, 0.2, 1.0})));

  scene.add( make_shared<Shape>( make_shared<Sphere>(Vec3::y_axis * -1000, 999),
			make_shared<RoughColorMaterial>(0, spectrum{0.2, 0.2, 0.2})));

  Vec3 light_dir = Vec3::z_axis;
  scene.add( make_shared<DirectionalLight>( light_dir, spectrum{1.0}));

  Vec3 light_pos = Vec3::z_axis * 6;
  scene.add( make_shared<DirectionalLight>( light_pos, spectrum{1.0}));

  return PerspectiveCamera(Vec3(0, 0, 2), Vec3::zero, Vec3::y_axis, PI / 2.0,
			   aspect_ratio);
}


PerspectiveCamera many_sphere_scene(Scene& scene, scalar ar)
{
  const int SPHERES_PER_SIDE = 10;
  for (int y = 0; y < SPHERES_PER_SIDE; ++y)
  {
    for (int x = 0; x < SPHERES_PER_SIDE; ++x)
    {
      auto color = spectrum::from_hsv(360.0 * y / SPHERES_PER_SIDE,
				      0.5 + 0.5 * x / SPHERES_PER_SIDE,
				      1.0);
      auto pos = Vec3{lerp<scalar>(-5, 5, x/(SPHERES_PER_SIDE-1.0)),
		      lerp<scalar>(-5, 5, y/(SPHERES_PER_SIDE-1.0)), 0.0};

      scene.add(make_shared<Shape>(make_shared<Sphere>(pos, 10.0/(2.1*SPHERES_PER_SIDE)),
			  make_shared<RoughColorMaterial>(1.0, color)));
    }
  }

  scene.add(make_shared<Shape>(make_shared<Plane>(Vec3::y_axis, 6.0),
		      make_shared<MirrorMaterial>()));
  // scene.add(make_shared<Shape>(make_shared<Plane>(Vec3{-1, 0, 1}, 10.0),
  //                     make_shared<MirrorMaterial>()));

  const scalar LP = 4.0;

  scene.add( make_shared<PointLight>( Vec3{-5, -5, -5}, spectrum{LP}) );
  scene.add( make_shared<PointLight>( Vec3{-5, 5, -5}, spectrum{LP}) );
  scene.add( make_shared<PointLight>( Vec3{5, -5, -5}, spectrum{LP}) );
  scene.add( make_shared<PointLight>( Vec3{5, 5, -5}, spectrum{LP}) );

  return PerspectiveCamera(Vec3{0,0,-7}, Vec3::zero, Vec3::y_axis, PI/2, ar);
}

PerspectiveCamera model_scene(Scene& scene, scalar aspect_ratio)
{
  RawModel m;
  if (!m.load_stl_model("cube.stl").success)
    exit(1);
  cerr << "Loaded model with " << m.verts.size() << " verts and " << m.tris.size() << " tris." << endl;
  //m.rescale(bounds::AABB(Vec3(-2, 0, -2), Vec3(2, 2, 2)));
  auto raw_mesh = new Mesh(m);
  auto mesh = shared_ptr<Mesh>(raw_mesh);

  scene.add(make_shared<Shape>(mesh, make_shared<RoughColorMaterial>(0.0, spectrum{1.0, 0.5, 0.0})));
  scene.add(make_shared<Shape>(make_shared<Plane>(Vec3::y_axis, 1.01), 
                      make_shared<RoughColorMaterial>(0.0, spectrum{0.5, 1.0, 0.0})));

  
  scene.add(make_shared<DirectionalLight>(Vec3(1.0, 1.0, 1.0), spectrum{1.0, 1.0, 1.0}*3.0));

  return PerspectiveCamera(Vec3{-2.0, 3.0, 6.0}, Vec3::zero, Vec3::y_axis,
                           PI/2.0, aspect_ratio);
}

PerspectiveCamera default_scene(Scene& scene, scalar aspect_ratio)
{
  auto mat  = make_shared<GlassMaterial>();

  scene.add(make_shared<Shape>(make_shared<Sphere>(Vec3{0.0, -0.5, 0.0}, 1.0), mat));

  scalar distance_from_center = 3.0;
  scalar sphere_radius = 1.0;

  const int num_sides = PI / atan(sphere_radius / (distance_from_center));
  for (int i = 0; i < num_sides; ++i)
  {
    const scalar angle = 2 * PI * i / num_sides;
    const scalar pr = 4.0;
    const Vec3 sp(pr*cos(angle), -2.0+sphere_radius, pr*sin(angle));
    auto sc = spectrum::from_hsv(i*360.0/num_sides, 1.0, 1.0);
    auto sc2 = spectrum::from_hsv(i*360.0/num_sides, 0.75, 0.75);

    scene.add( make_shared<Shape>( make_shared<Sphere>(sp, sphere_radius),
                                   make_shared<RoughMaterial>(0.0, make_shared<GridTexture2D>(sc, sc2, 30.0, 0.1))));
  }

  scene.add(make_shared<Shape>(make_shared<Plane>(Vec3{0.0, 1.0, 0.0}, 2.0),
                               make_shared<RoughColorMaterial>(0.0, spectrum{0.6})));

  //scene.add(make_shared<PointLight>(Vec3(3.0, 3.0, -1.0), spectrum{5.0}));
  scene.add(make_shared<EnvironmentalLight>(make_shared<Checkerboard2D>(spectrum{1.0}, spectrum{0.0}, 8, 1)));

  const int num_lights = 0;


  for (int i = 0; i < num_lights; ++i)
  {
    const scalar angle = 2 * PI * i / num_lights + PI/12;
    const scalar pr = 4.0;
    scene.add( make_shared<PointLight>(Vec3(pr*cos(angle), 2.0, pr*sin(angle)), spectrum{0.1}));
  }

  auto cam_pos = Vec3{7.5, 2, 0}*0.8;

  return PerspectiveCamera(cam_pos, Vec3{0, -1.0, 0}, Vec3{0, 1, 0}, PI / 2.0, aspect_ratio);
  //return PerspectiveCamera(Vec3{0, 4, 0}, Vec3{0, 5.0, 0}, Vec3::x_axis, PI / 2.0, aspect_ratio);
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
  opt.max_depth = 16;

  PathTracerIntegrator igr(opt);

  //BWIntegrator igr;

  cerr << "Rendering image at " << WIDTH << "x" << HEIGHT << " resolution, "
       << per_pixel << " samples per pixel\n";

  igr.render(&cam, &scene, f);

  cerr << "Rendered " << igr.num_primary_rays_traced() << " rays.\n";

  //auto mapper = make_shared<LinearToneMapper>();
  auto mapper = shared_ptr<CompositeToneMapper>(new CompositeToneMapper{make_shared<RSSFToneMapper>(), make_shared<LinearToneMapper>()});
  f.render_to_ppm(cout, mapper);
  //f.render_to_console(cout);

  return 0;
}
