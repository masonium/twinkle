#include "basic_scenes.h"
#include "geometries.h"
#include "textures/skytexture.h"
#include "script/proc_texture.h"
#include "env_light.h"
#include "materials/rough_glass_material.h"
#include "util/timer.h"

using std::shared_ptr;
using std::make_shared;

shared_ptr<Shape> SHAPE(shared_ptr<Geometry> x, shared_ptr<Material> y)
{
  return make_shared<Shape>(x, y);
}

shared_ptr<Material> COLOR(const spectrum& x)
{
  return make_shared<RoughColorMaterial>(0, x);
}

shared_ptr<Camera> empty_scene(Scene& scene, scalar aspect_ratio)
{
  auto sun_dir = Vec3::from_euler(PI/4, PI/4);
  scene.add(make_shared<EnvironmentalLight>(make_shared<ShirleySkyTexture>(sun_dir, 8)));

  return make_shared<SphericalCamera>(Vec3::zero, -Vec3::y_axis, Vec3::z_axis);
}

shared_ptr<Camera> glass_scene(Scene& scene, scalar aspect_ratio)
{
  //auto glass = make_shared<GlassMaterial>();
  auto glass = make_glass_material(RoughGlassDistribution::GGX, 0.01);
  //auto glass = make_shared<MirrorMaterial>();
  //scene.add( make_shared<Shape>( make_quad(Vec3::x_axis, Vec3::y_axis), glass ));
  scene.add( make_shared<Shape>( make_shared<Sphere>(Vec3::zero, 0.8), glass ));

  auto check = make_shared<Checkerboard2D>(spectrum{0.1}, spectrum{0.9}, 2.0);

  scene.add( make_shared<Shape>( make_shared<Plane>(Vec3::y_axis, 1.0),
                                 make_shared<RoughMaterial>(0, check)));

  scene.add(SHAPE(make_shared<Sphere>(Vec3{1.0, 0, -3.0}, 1.0), COLOR(spectrum{0.1, 0.6, 0.1})));

  // Vec3 light_dir = Vec3::z_axis;
  // scene.add( make_shared<DirectionalLight>( light_dir, spectrum{1.0}));

  // Vec3 light_pos = Vec3::z_axis * 6;
  // scene.add( make_shared<DirectionalLight>( light_pos, spectrum{1.0}));

  scene.add(make_shared<EnvironmentalLight>(make_shared<SolidColor>(spectrum{1.0})));

  Vec3 cam_pos(1.0, 1.0, 2);
  return make_shared<PerspectiveCamera>(cam_pos, Vec3::zero, Vec3::y_axis, PI / 2.0,
                                        aspect_ratio);
}

shared_ptr<Camera> box_scene(Scene& scene, scalar ar, scalar angle)
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

  return make_shared<PerspectiveCamera>(Vec3(7*sin(angle * PI / 180), 0, 7*cos(angle * PI / 180)), Vec3::zero, Vec3::y_axis,
                                        PI/2.0, ar);
}

shared_ptr<Camera> many_sphere_scene(Scene& scene, scalar ar, int angle)
{
  const int SPHERES_PER_SIDE = 12;
  const scalar SW = 6.0;

  for (int y = 0; y < SPHERES_PER_SIDE; ++y)
  {
    for (int x = 0; x < SPHERES_PER_SIDE; ++x)
    {
      auto color = spectrum::from_hsv(360.0 * y / SPHERES_PER_SIDE,
				      0.5 + 0.5 * x / SPHERES_PER_SIDE,
				      1.0);
      auto pos = Vec3{lerp<scalar>(-SW, SW, x/(SPHERES_PER_SIDE-1.0)),
		      lerp<scalar>(-SW, SW, y/(SPHERES_PER_SIDE-1.0)), 0.0};

      scene.add(make_shared<Shape>(make_shared<Sphere>(pos, 10.0/(2.1*SPHERES_PER_SIDE)),
                                   make_shared<RoughColorMaterial>(1.0, color)));
    }
  }

  // scene.add(make_shared<Shape>(make_shared<Plane>(Vec3::y_axis, 6.0),
  //                              make_shared<MirrorMaterial>()));
  scene.add(make_shared<Shape>(make_shared<Plane>(Vec3{-1, 0, 1}, 10.0),
                               make_shared<MirrorMaterial>()));

  const scalar LP = 4.0;

  scene.add( make_shared<PointLight>( Vec3{-SW, -SW, -SW}, spectrum{LP}) );
  scene.add( make_shared<PointLight>( Vec3{-SW, SW, -SW}, spectrum{LP}) );
  scene.add( make_shared<PointLight>( Vec3{SW, -SW, -SW}, spectrum{LP}) );
  scene.add( make_shared<PointLight>( Vec3{SW, SW, -SW}, spectrum{LP}) );

  return make_shared<PerspectiveCamera>(
    12.0 * Vec3(sin(angle * PI/180), 0, -cos(angle*PI/180)),
    Vec3::zero, Vec3::y_axis, PI/2, ar);
}

shared_ptr<Camera> model_scene(Scene& scene, scalar aspect_ratio,
                               const string& model_filename, bool invert)
{
  RawModel m;
  Timer tm;
  if (!m.load_obj_model(model_filename).success)
  {
    cerr << "could not load model." << model_filename << endl;
    exit(1);
  }
  cerr << "Loaded " << model_filename << " in " << format_duration(tm.since()) << endl;
  cerr << "Model has " << m.tris().size() << " triangles." << endl;

  m.translate_to_origin();
  m.rescale(bounds::AABB(Vec3(-1.0, 5.0, -1.0), Vec3(1.0, 7.0, 1.0)), true);

  if (invert)
    m.flip_tris();

  //auto mesh = rotate(make_shared<KDMesh>(m), Vec3::y_axis, PI/4);;
  tm.reset();
  auto mesh = make_shared<KDMesh>(m);
  cerr << "Created KDMesh in " << format_duration(tm.since()) << endl;
  cerr << "Mesh has " << mesh->kd_tree->count_objects() << " objects among "
       << mesh->kd_tree->count_leaves() << " leaves." << endl;
  //auto mesh = translate(make_quad(), Vec3{0.0, 0.0, 0.0});

  auto rcm = make_shared<RoughColorMaterial>(0.0, spectrum{1.0, 0.5, 0.0});
  auto nmc = make_shared<RoughMaterial>(0.00, make_shared<NormalTexture>());

  UniformSampler sampler;

  for (int i = 0; i < 27; ++i)
  {
    int x = i % 3 - 1;
    int y = (i / 3) % 3 - 1;
    int z = i / 9 - 1;
    Vec3 r = uniform_sphere_sample(sampler.sample_2d());
    auto tr_mesh = rotate(translate(mesh, Vec3(x, y, z) * 3.1), r, sampler.sample_1d() * PI);
    scene.add(make_shared<Shape>(tr_mesh, nmc));
  }

  //auto plane_mat = make_shared<RoughColorMaterial>(0.0, spectrum{0.2, 0.7, 0.0});
  auto plane_mat = make_shared<RoughMaterial>(0.0, make_shared<ScriptTexture>("checker"));
  scene.add(make_shared<Shape>(make_shared<Plane>(Vec3::y_axis, 2), plane_mat));

  scene.add(make_shared<EnvironmentalLight>(make_shared<SolidColor>(spectrum{5.0})));

  return make_shared<PerspectiveCamera>(Vec3{2.0, 7.0, 7.0}, Vec3(0, 6, 0), Vec3::y_axis,
                           PI/2.0, aspect_ratio);
}


shared_ptr<Camera> default_scene(Scene& scene, scalar aspect_ratio, int angle)
{
  auto mirror = make_shared<MirrorMaterial>();

  auto sphere = make_shared<Sphere>(Vec3{0.0, 0.0, 0.0}, 1.0);

  scene.add(make_shared<Shape>(sphere, make_shared<RoughMaterial>(0.0, make_shared<NormalTexture>())));

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

  auto plane_mat = make_shared<RoughColorMaterial>(0.0, spectrum{0.6});

  scene.add(make_shared<Shape>(make_shared<Plane>(Vec3{0.0, 0.0, 1.0}, sphere_radius),
                               plane_mat));

  auto env_light_tex = make_shared<Checkerboard2D>(spectrum{1.0}, spectrum{0.0}, 2, 1);
  //auto env_light_tex = make_shared<SolidColor>(spectrum{1.0});
  //auto env_light_tex = make_shared<ShirleySkyTexture>(Vec3{0, 0, 1}, 20);
  scene.add(make_shared<EnvironmentalLight>(env_light_tex));

  auto cam_pos = Mat33::from_axis_angle(Vec3::z_axis, angle * PI / 180) * Vec3{0, 7.5, 2.0};
  auto look_at = Vec3{0.0, 0.0, 0.0};

  return make_shared<PerspectiveCamera>(cam_pos, look_at, Vec3{0, 0, 1}, PI/2, aspect_ratio);
}

shared_ptr<Camera> showcase_scene(Scene& scene, scalar ar, int angle)
{
  vector<shared_ptr<Geometry>> geoms;

  auto cmat = make_shared<RoughMaterial>(0.0, make_shared<NormalTexture>());
  auto mirr_mat = make_shared<MirrorMaterial>();
  geoms.push_back(make_shared<Sphere>(Vec3::zero, 0.5));

  RawModel model;
  auto filename = "/home/mason/workspace/twinkle/assets/models/tak-cube.obj";
  if (!model.load_obj_model(filename).success)
  {
    cerr << "could not load " << filename << endl;
    exit(1);
  }

  model.rescale(bounds::AABB(Vec3{-0.5}, Vec3{0.5}), true);

  geoms.push_back(make_shared<KDMesh>(model));

  geoms.push_back(make_quad(Vec3::z_axis * 0.5, Vec3::x_axis * 0.5));

  geoms.push_back(make_torus(Vec3::y_axis, 0.8, 0.2));
  geoms.push_back(make_capsule(Vec3::x_axis, 0.7, 0.15));
  geoms.push_back(make_rounded_box(Vec3{0.6}, 0.4));

  const scalar spacing = 2.0;
  const auto total_shapes = geoms.size() * 2;
  int grid_size = ceil(sqrtf(total_shapes));

  UniformSampler sampler;
  {
    auto i = 0u;
    for (auto j = 0u, x = 0u, z = 0u; j < geoms.size(); ++j, ++i, x = i % grid_size, z = i / grid_size)
    {
      scene.add(SHAPE(translate(geoms[j], spacing * Vec3{scalar(x), 0, scalar(z)}), cmat));
    }

    for (auto j = 0u, x = 0u, z = 0u; j < geoms.size(); ++j, ++i, x = i % grid_size, z = i / grid_size)
    {
      auto r = rotate(geoms[j], uniform_hemisphere_sample(sampler.sample_2d()), sampler.sample_1d() * 2 * PI);
      scene.add(SHAPE(translate(r, spacing * Vec3{scalar(x), 0, scalar(z)}), mirr_mat));
    }
  }

  //auto plane_mat = make_shared<RoughColorMaterial>(0.0, spectrum{0.8});
  auto plane_mat = make_shared<RoughMaterial>(0.0, make_shared<Checkerboard2D>(spectrum{0.0}, spectrum{1.0}, 2));
  scene.add(SHAPE(make_shared<Plane>(Vec3::y_axis, 1.2), plane_mat));

  scalar gs = (grid_size - 1) * spacing ;

  scene.add(make_shared<PointLight>( Vec3{0, gs, -gs/2}, spectrum{2.0} ));
  scene.add(make_shared<EnvironmentalLight>( make_shared<SolidColor>(spectrum{0.0})));

  return make_shared<PerspectiveCamera>(Vec3(gs * 0.5, gs * .7, -gs * .4), Vec3{gs/2, 0, gs/4}, Vec3::y_axis, PI/2.0, ar);
}
