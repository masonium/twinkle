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
#include "materials/ggx.h"

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
  auto g = make_shared<Sphere>(Vec3{0.0, 0.0, 0.0}, 1.0);

  Shape shape(g.get(), m.get());
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

void check_ggx(scalar roughness)
{
  int N = 10000;
  int G = 10;
  GGX ggx(roughness);
  Vec3 incoming = Vec3::z_axis;

  UniformSampler us;

  Array2D<int> hist(G, G);
  fill(hist.begin(), hist.end(), 0);

  for (int i = 0; i < N; ++i)
  {
    auto m = ggx.sample_micronormal(incoming, us);
    auto angles = m.to_euler();
    hist(int(angles.first * G / (2*M_PI)), int(angles.second * G / (M_PI/2)))++;
  }

  for (int y = 0; y < G; ++y)
  {
    for (int x = 0; x < G; ++x)
      cout << hist(x, y) << " ";
    cout << "\n";
  }

  for (int y = 0; y < G; ++y)
  {
    scalar up = 0.5 * M_PI * (y + 0.5) / G;
    for (int x = 0; x < G; ++x)
    {
      scalar tp = 2 * M_PI * (x + 0.5) / G;
      Vec3 m = Vec3::from_euler(tp, up);
      //cout << m << "\n";
      cout << N * ggx.pdf_micronormal(incoming, m) << " ";
    }
    cout << "\n";
  }

}

void check_ggx_x(scalar roughness)
{
  int N = 10000;
  int G = 10;
  GGX ggx(roughness);
  Vec3 incoming = Vec3(1.0, 1.0, 0.3).normal();

  UniformSampler us;

  vector<int> hist(G);
  fill(hist.begin(), hist.end(), 0);

  for (int i = 0; i < N; ++i)
  {
    scalar xs = ggx.sample_marginal_x_slope(acos(incoming.z), us.sample_1d());
    scalar theta = atan(xs);
    hist[(theta/PI + 0.5)*G] ++;
  }

  for (int i = 0; i < G; ++i)
    cout << hist[i] << " ";
  cout << "\n";

  for (int i = 0; i < G; ++i)
  {
    scalar theta_i = M_PI * ( (i + 0.0) / G - 0.5);
    scalar theta_j = M_PI * ( (i + 1.0) / G - 0.5);

    cout << N * (ggx.cdf_marginal_x_slope(incoming, theta_j) - ggx.cdf_marginal_x_slope(incoming, theta_i)) << " ";
  }
  cout << "\n";
}



auto parse_args(int argc, char** args)
{
  optparse::OptionParser parser;

  parser.add_option("-w", "--width").action("store").type("int").set_default(512);
  parser.add_option("--height").action("store").type("int").set_default(512);

  parser.add_option("-r", "--roughness").action("store").type("float").set_default(0.0);
  parser.add_option("output_file").action("store").set_default("");

  return parser.parse_args(argc, args);
}

int main(int argc, char** args)
{
  auto opt = parse_args(argc, args);
  check_ggx_x(opt.get("roughness").as<float>());

  // auto tex = make_shared<SolidColor>(spectrum{0.2, 0.1, 0.8});
  // auto base_mat = make_shared<RoughMaterial>(opt.get("roughness").as<float>(), tex.get());

  // auto layer = make_shared<MFLayer>(0.0, spectrum{1.0}, make_shared<GTR>(0.1));

  // vector<decltype(layer)> layers({layer});
  // auto mat = make_shared<LayeredMFMaterial>(layers, base_mat.get());

  // Film film(opt.get("width").as<int>(), opt.get("height").as<int>());

  // render_material_scene(mat, film);

  // save_image(opt.get("output_file"), LinearToneMapper().tonemap(film.image()));

  return 0;
}
