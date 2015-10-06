#include "basic_scenes.h"
#include "geometries.h"
#include "textures/skytexture.h"

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

shared_ptr<Camera> single_sphere(Scene& scene, scalar aspect_ratio)
{
  scene.add( make_shared<Shape>( make_shared<Sphere>(Vec3::zero, 1.0),
                                 make_shared<RoughColorMaterial>(0, spectrum{0.2, 0.2, 1.0})));

  scene.add( make_shared<Shape>( make_shared<Plane>(Vec3::y_axis, 1.0),
                                 make_shared<RoughColorMaterial>(0, spectrum{0.2, 0.2, 0.2})));

  Vec3 light_dir = Vec3::z_axis;
  scene.add( make_shared<DirectionalLight>( light_dir, spectrum{1.0}));

  Vec3 light_pos = Vec3::z_axis * 6;
  scene.add( make_shared<DirectionalLight>( light_pos, spectrum{1.0}));

  return make_shared<PerspectiveCamera>(Vec3(0, 0, 2), Vec3::zero, Vec3::y_axis, PI / 2.0,
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

shared_ptr<Camera> model_scene(Scene& scene, scalar aspect_ratio)
{
  RawModel m;
  if (!m.load_obj_model("/home/mason/workspace/twinkle/teapot.obj").success)
  {
    cerr << "could not load model." << endl;
    exit(1);
  }
  m.translate_to_origin();
  m.rescale(bounds::AABB(Vec3(-2.0), Vec3(2.0)), true);

  //auto mesh = rotate(make_shared<KDMesh>(m), Vec3::y_axis, PI/4);;
  auto mesh = make_shared<KDMesh>(m);
  //auto mesh = make_shared<Sphere>(Vec3{0.0, 1.0, 0.0}, 3.1);
  //auto mesh2 = make_shared<Sphere>(Vec3{0.0, 1.0, 8.0}, 3.1);
  //auto mesh = translate(make_quad(), Vec3{0.0, 0.0, 0.0});

  auto rcm = make_shared<RoughColorMaterial>(0.0, spectrum{1.0, 0.5, 0.0});
  auto nmc = make_shared<RoughMaterial>(0.00, make_shared<NormalTexture>());

  scene.add(make_shared<Shape>(mesh, nmc));

  auto green = make_shared<RoughColorMaterial>(0.0, spectrum{0.2, 0.7, 0.0});
  scene.add(make_shared<Shape>(make_shared<Plane>(Vec3::y_axis, 2), green));

  scene.add(make_shared<EnvironmentalLight>(make_shared<SolidColor>(spectrum{5.0})));

  return make_shared<PerspectiveCamera>(Vec3{2.0, 1.0, 5.0}, Vec3::zero, Vec3::y_axis,
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

shared_ptr<Camera> showcase_scene(Scene& scene, scalar ar, int angle)
{
  vector<shared_ptr<Geometry>> geoms;

  auto cmat = make_shared<RoughMaterial>(0.0, make_shared<NormalTexture>());
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
  geoms.push_back(make_rounded_box(Vec3{0.8}, 0.2));

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
      scene.add(SHAPE(translate(r, spacing * Vec3{scalar(x), 0, scalar(z)}), cmat));
    }
  }
  auto gray = make_shared<RoughColorMaterial>(0.0, spectrum{0.8});
  scene.add(SHAPE(make_shared<Plane>(Vec3::y_axis, 1.2), gray));

  scalar gs = (grid_size - 1) * spacing ;

  scene.add(make_shared<PointLight>( Vec3{0, gs, 0}, spectrum{2.0} ));
  scene.add(make_shared<EnvironmentalLight>( make_shared<SolidColor>(spectrum{3.0})));


  return make_shared<PerspectiveCamera>(Vec3(gs * 0.5, gs * .8, -gs * .4), Vec3{gs/2, 0, gs/4}, Vec3::y_axis, PI/2.0, ar);
}
