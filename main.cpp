#include <iostream>
#include <memory>
#include <sstream>

#include "camera.h"
#include "integrator.h"
#include "dintegrator.h"
#include "scene.h"
#include "bsdf.h"
#include "path_tracer.h"
#include "directlightingintegrator.h"
#include "util.h"
#include "model.h"
#include "material.h"
#include "shapes/kdmesh.h"
#include "shapes/mesh.h"
#include "shapes/sphere.h"
#include "shapes/plane.h"
#include "shapes/implicit.h"
#include "shapes/transformed.h"
#include "shapes/quad.h"
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

shared_ptr<Camera> empty_scene(Scene& scene, scalar aspect_ratio)
{
  auto sun_dir = Vec3::from_euler(PI/4, PI/4);
  scene.add(make_shared<EnvironmentalLight>(make_shared<ShirleySkyTexture>(sun_dir, 8)));

  return make_shared<SphericalCamera>(Vec3::zero, -Vec3::y_axis, Vec3::z_axis);
}

PerspectiveCamera single_sphere(Scene& scene, scalar aspect_ratio)
{
  scene.add( make_shared<Shape>( make_shared<Sphere>(Vec3::zero, 1.0),
                                 make_shared<RoughColorMaterial>(0, spectrum{0.2, 0.2, 1.0})));

  scene.add( make_shared<Shape>( make_shared<Plane>(Vec3::y_axis, 1.0),
                                 make_shared<RoughColorMaterial>(0, spectrum{0.2, 0.2, 0.2})));

  Vec3 light_dir = Vec3::z_axis;
  scene.add( make_shared<DirectionalLight>( light_dir, spectrum{1.0}));

  Vec3 light_pos = Vec3::z_axis * 6;
  scene.add( make_shared<DirectionalLight>( light_pos, spectrum{1.0}));

  return PerspectiveCamera(Vec3(0, 0, 2), Vec3::zero, Vec3::y_axis, PI / 2.0,
			   aspect_ratio);
}

shared_ptr<Shape> SHAPE(shared_ptr<Geometry> x, shared_ptr<Material> y)
{
  return make_shared<Shape>(x, y);
}

shared_ptr<Material> COLOR(const spectrum& x) 
{
  return make_shared<RoughColorMaterial>(0, x);
}

PerspectiveCamera box_scene(Scene& scene, scalar ar, scalar angle)
{
  // right
  scene.add(SHAPE(translate(make_quad(Vec3::z_axis * 3.0, Vec3::y_axis * 3.0),
                            Vec3::x_axis * 3.0),
                  COLOR(spectrum{0.1, 0.1, 0.6})));
  
  // left
  scene.add(SHAPE(translate(make_quad(Vec3::z_axis * 3.0, Vec3::y_axis * -3.0),
                            Vec3::x_axis * -3.0), 
                  COLOR(spectrum{0.6, 0.1, 0.1})));

  // top 
  scene.add(SHAPE(translate(make_quad(Vec3::x_axis * 3.0, Vec3::z_axis * 3.0),
                            Vec3::y_axis * 3.0), 
                  COLOR(spectrum{1.0})));

  // back
  scene.add(SHAPE(translate(make_quad(Vec3::x_axis * 3.0, Vec3::y_axis * 3.0),
                            Vec3::z_axis * -3.0), 
                  COLOR(spectrum{1.0})));

  // botttom (plane)
  scene.add(SHAPE(make_shared<Plane>(Vec3::y_axis, 1.5),
                  COLOR(spectrum{1.0})));

  scene.add(make_shared<PointLight>(Vec3{-5.0, 5.0, 5.0},
                                    spectrum{3.0}));
  // scene.add(make_shared<PointLight>(Vec3{0, 2.9, 0},
  //                                   spectrum{3.0}));
  scene.add(make_shared<EnvironmentalLight>(make_shared<SolidColor>(spectrum{4.0})));
                  
  return PerspectiveCamera(Vec3(7*sin(angle * PI / 180), 0, 7*cos(angle * PI / 180)), Vec3::zero, Vec3::y_axis,
                           PI/2.0, ar);
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
  cerr << "Loaded model with " << m.verts.size() << " verts and " <<
    m.tris.size() << " tris." << endl;


  //m.rescale(bounds::AABB(Vec3(-2, 0, -2), Vec3(2, 2, 2)));

  //auto mesh = rotate(make_shared<KDMesh>(m), Vec3::y_axis, PI/4);;
  auto mesh = translate(make_quad(), Vec3{0.0, 0.0, 0.0});

  // cerr << "Created kdmesh with " << kdmesh->kd_tree->count_leaves() << " leaves and "
  //      << kdmesh->kd_tree->count_objs() << " objects.\n";

  auto rcm = make_shared<RoughColorMaterial>(0.0, spectrum{1.0, 0.5, 0.0});
  auto mc = make_shared<RoughMaterial>(
    0.0, make_shared<Gradient2D>());
  auto mirror = make_shared<MirrorMaterial>();
  scene.add(make_shared<Shape>(mesh, mc));
  
  auto green = make_shared<RoughColorMaterial>(0.0, spectrum{0.2, 0.7, 0.0});
  scene.add(make_shared<Shape>(make_shared<Plane>(Vec3::y_axis, 1.01), green));
  
  auto check = make_shared<Checkerboard2D>(spectrum{1.0}, spectrum{0.0}, 2, 1);
  scene.add(make_shared<EnvironmentalLight>(make_shared<SolidColor>(spectrum{1.0})));

  return PerspectiveCamera(Vec3{0.0, 3.0, -4.0}, Vec3::zero, Vec3::y_axis,
                           PI/2.0, aspect_ratio);
}

scalar sphere_sdf(Vec3 v)
{
  return v.norm() - 1.0;
}

scalar torus_sdf(Vec3 v)
{
  scalar xz = sqrt(v.x*v.x + v.y*v.y);
  scalar x = xz - 0.2;
  scalar y = v.y;
  return sqrt(x*x+y*y) - 1.0;
}
scalar capsule_sdf(Vec3 v)
{
  Vec3 a{0.0, 0.0, -1.0};
  Vec3 b{0.0, 0.0, 1.0};
  
  Vec3 va  = v - a, ba = b - a;
  scalar s = clamp( va.dot(ba) / ba.norm2(), 0, 1);
  
  return ( va - ba * s ).norm() - 0.1;
}

shared_ptr<Camera> default_scene(Scene& scene, scalar aspect_ratio, int angle)
{
  auto glass = make_shared<GlassMaterial>();
  auto mirror = make_shared<MirrorMaterial>();

  auto impf = sphere_sdf;
  auto implicit = make_shared<ImplicitSurface>(impf, gradient_from_sdf(impf), 1.0);
  auto sphere = make_shared<Sphere>(Vec3{0.0, 0.0, 0.0}, 1.0);
  //scene.add(make_shared<Shape>(implicit, glass));

  scalar distance_from_center = 5.0;
  scalar sphere_radius = 1.0;
  scalar fill_rate = 0.75;

  const int num_sides = fill_rate * PI / atan(sphere_radius / (distance_from_center));

  for (int i = 0; i < num_sides; ++i)
  {
    const scalar angle = 2 * PI * i / num_sides;
    const scalar pr = distance_from_center;
    const Vec3 sp(pr*cos(angle), pr*sin(angle), 0);
    auto sc = spectrum::from_hsv(i*360.0/num_sides, 1.0, 1.0);
    auto sc2 = spectrum::from_hsv(i*360.0/num_sides, 0.75, 0.75);

    scene.add( make_shared<Shape>( make_shared<Sphere>(sp, sphere_radius),
                                   make_shared<RoughMaterial>(0.0, make_shared<GridTexture2D>(sc, sc2, 30.0, 0.1))));
  }

  scene.add(make_shared<Shape>(make_shared<Plane>(Vec3{0.0, 0.0, 1.0}, sphere_radius),
                               make_shared<RoughColorMaterial>(0.0, spectrum{0.6})));

  auto env_light_tex = make_shared<Checkerboard2D>(spectrum{1.0}, spectrum{0.0}, 2, 1);
  //auto env_light_tex = make_shared<SolidColor>(spectrum{1.0});
  //auto env_light_tex = make_shared<ShirleySkyTexture>(Vec3{0, 0, 1}, 20);
  scene.add(make_shared<EnvironmentalLight>(env_light_tex));

  auto cam_pos = Mat33::from_axis_angle(Vec3::z_axis, angle * PI / 180) * Vec3{0, 7.5, 2.0};
  auto look_at = Vec3{0.0, 0.0, 0.0};

  //return make_shared<SphericalCamera>(cam_pos, look_at, Vec3{0, 0, 1});
  return make_shared<PerspectiveCamera>(cam_pos, look_at, Vec3{0, 0, 1}, PI/2, aspect_ratio);
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

  Scene scene;
  int angle = atoi(args[5]);
  auto cam = default_scene(scene, scalar(WIDTH)/scalar(HEIGHT), angle);

  auto bf = make_shared<BoxFilter>();
  Film f(WIDTH, HEIGHT, bf);

  // PathTracerIntegrator::Options opt;
  // opt.samples_per_pixel = per_pixel;
  // if (argc >= 5)
  //   opt.num_threads = atoi(args[4]);
  // else
  //   opt.num_threads = 0;
  // opt.max_depth = 16;
  // PathTracerIntegrator igr(opt);

  DirectLightingIntegrator::Options opt;
  opt.samples_per_pixel = 4;
  opt.lighting_samples = 16;
  opt.subdivision = atoi(args[5]);
  opt.num_threads = 4;
  DirectLightingIntegrator igr(opt);
  
  //DebugIntegrator igr(DebugIntegrator::DI_NORMAL);

  // cerr << "Rendering image at " << WIDTH << "x" << HEIGHT << " resolution, "
  //      << per_pixel << " samples per pixel\n";

  igr.render(*cam, scene, f);

  // auto mapper = make_shared<LinearToneMapper>();
  auto mapper = make_shared<ReinhardGlobal>();
  //auto mapper = make_shared<ReinhardLocal>(ReinhardLocal::Options{});
  //auto mapper = make_shared<CutoffToneMapper>();

  f.render_to_ppm(cout, *mapper);
  //f.render_to_twi(cout);
  //f.render_to_console(cout);

  return 0;
}
