#include "material.h"
#include "materials/multilayered.h"
#include "film.h"
#include "geometries.h"
#include "camera.h"
#include "light.h"
#include "textures.h"
#include "reinhard.h"
#include <iostream>
#include <vector>
#include <memory>
#include "cpp-optparse/OptionParser.h"

using std::make_shared;
using std::vector;
using std::cout;
using std::endl;

/*
 * The rendering is based on a hemisphere. of radius one, looked on by a camera at a
 * distance sqrt(2) away.
 */
void render_material_scene(shared_ptr<Material> m, Film& film)
{

  Shape shape(make_shared<Sphere>(Vec3{0.0, 0.0, 0.0}, 1.0), m);
  PerspectiveCamera cam(Vec3{0.0, 0.0, sqrt(2.0)}, Vec3::zero, Vec3::y_axis, PI/2);

  ConstSampler sampler(0.5, 0.5, 0.5, 0.5, 0.5);

  PointLight pl({0.5, 0.0, sqrt(2)}, spectrum{25.0});

  for (auto y = 0u; y < film.height; ++y)
  {
    for (auto x = 0u; x < film.width; ++x)
    {
      auto ps = cam.sample_pixel(film.width, film.height, x, y, sampler);

      SubGeo ignore;
      scalar_fp max_t = sfp_none;
      auto f = shape.intersect(ps.ray, max_t, ignore);
      if (!f.is())
      {
        film.add_sample(ps, spectrum{0.0});
        continue;
      }

      scalar t = f.get();
      Intersection isect(&shape, SUBGEO_NONE, ps.ray, t);

      auto light_em = pl.sample_emission(isect, sampler);
      auto L = light_em.direction();
      auto N = isect.normal;
      if (N.dot(L) > 0)
      {
        auto refl = m->reflectance(isect, -ps.ray.direction.normal(), light_em.direction().normal());
        spectrum contrib = N.dot(L) * light_em.emission() * refl;
        film.add_sample(ps, contrib);
      }
      else
      {
        film.add_sample(ps, spectrum{0.0});
      }
    }
  }
}

auto parse_args(int argc, char** args)
{
  optparse::OptionParser parser;

  parser.add_option("-w", "--width").action("store").type("int").set_default(512);
  parser.add_option("--height").action("store").type("int").set_default(512);

  parser.add_option("-r", "--roughness").action("store").type("float").set_default(0.0);

  return parser.parse_args(argc, args);
}

int main(int argc, char** args)
{
  auto opt = parse_args(argc, args);

  // auto tex = make_shared<Checkerboard2D>(spectrum{0.6, 0.1, 0.2},
  //                                        spectrum{0.2, 0.1, 0.6}, 16.0);
  auto tex = make_shared<SolidColor>(spectrum{0.2, 0.1, 0.8});
  auto base_mat = make_shared<RoughMaterial>(opt.get("roughness").as<float>(), tex);

  auto layer = make_shared<MFLayer>(0.0, spectrum{1.0}, make_shared<GTR>(0.1));

  vector<decltype(layer)> layers({layer});
  auto mat = make_shared<LayeredMFMaterial>(layers, base_mat);
  // auto mat = make_shared<MaterialTLDAdapter>(
  //   refraction_index::AIR, refraction_index::CROWN_GLASS, make_shared<GTR>(0.2));

  Film film(opt.get("width").as<int>(), opt.get("height").as<int>());

  render_material_scene(mat, film);

  //ReinhardLocal::Options rl_opt;
  //film.render_to_ppm(cout, ReinhardGlobal());
  film.render_to_ppm(cout, LinearToneMapper());

  return 0;
}
