#include <iostream>
#include <iomanip>
#include <cassert>
#include "textures.h"
#include "geometries.h"
#include "sampler.h"
#include "vec2.h"
#include "bsdf.h"


using namespace std;

struct sas
{
  sas() : tp(0), n(0) { }
  scalar tp;
  int n;
};

scalar solid_angle_area(scalar t0, scalar t1, scalar p0, scalar p1)
{
  return (t1 - t0) * (cos(p0) - cos(p1));
}

scalar solid_angle_weighted_area(scalar t0, scalar t1, scalar p0, scalar p1)
{
  return (t1 - t0) * (cos(2 * p0) - cos(2 * p1)) * 0.5;
}

void compare_bsdf(const BRDF& brdf, uint num_samples)
{
  UniformSampler sampler;

  Vec3 incoming_dir = Vec3{0.0f, sqrt(0.5), sqrt(0.5)};

  const int theta_grid = 1;
  const int phi_grid = 1000;

  sas r[theta_grid][phi_grid];

  for (auto i = 0u; i < num_samples; ++i)
  {
    // compute the pdf
    scalar p;
    scalar refl;
    Vec3 dir = brdf.sample(incoming_dir, sampler, p, refl);

    scalar theta, phi;
    dir.to_euler(theta, phi);

    int theta_i = theta / (2 * PI) * theta_grid ;
    int phi_i = phi / (PI / 2) * phi_grid ;
    assert(0 <= theta_i  && theta_i < theta_grid);
    assert(0 <= phi_i  && phi_i < phi_grid);
    r[theta_i][phi_i].n += 1;
    r[theta_i][phi_i].tp += p;
  }

  const int num_cells = theta_grid * phi_grid;

  scalar total_integral = 0;
  for (int y = 0; y < theta_grid; ++y)
  {
    for (int x = 0; x < phi_grid; ++x)
    {
      scalar disp = num_cells * scalar(r[y][x].n) / num_samples;
      cout << setw(12) << disp;
      total_integral += disp;
    }
    cout << endl;
  }
  cout << total_integral / num_cells << endl << endl;

  total_integral = 0;
  for (int y = 0; y < theta_grid; ++y)
  {
    scalar t0 = y * 2 * PI / theta_grid;
    scalar t1 = (y + 1)* 2 * PI / theta_grid;
    for (int x = 0; x < phi_grid; ++x)
    {
      scalar p0 = x * 0.5 * PI / phi_grid;
      scalar p1 = (x + 1) * 0.5 * PI / phi_grid;
      scalar disp = r[y][x].n == 0 ? 0 : num_cells * (r[y][x].tp / r[y][x].n) * solid_angle_area(t0, t1, p0, p1);
      cout << setw(12) << disp;
      total_integral += disp;
    }
    cout << endl;
  }
  cout << total_integral / num_cells << endl << endl;


  total_integral = 0;
  for (int y = 0; y < theta_grid; ++y)
  {
    scalar t0 = y * 2 * PI / theta_grid;
    scalar t1 = (y + 1)* 2 * PI / theta_grid;
    for (int x = 0; x < phi_grid; ++x)
    {
      scalar p0 = x * 0.5 * PI / phi_grid;
      scalar p1 = (x + 1) * 0.5 * PI / phi_grid;
      scalar a = r[y][x].n == 0 ? 0 : num_cells * (r[y][x].tp / r[y][x].n) * solid_angle_area(t0, t1, p0, p1);
      scalar b = num_cells * scalar(r[y][x].n) / num_samples;
      scalar disp = a - b;
      cout << setw(12) << disp * 100;
      total_integral += disp;
    }
    cout << endl;
  }
  cout << total_integral / num_cells << endl << endl;
}



scalar compute_rho_hd(const Material* mat, const Vec3& incoming, uint num_samples)
{
  spectrum r;

  auto p = make_shared<Plane>(Vec3(0, 0, 1), 0.0);
  Shape s(p.get(), mat);
  Ray ray(incoming, -incoming);
  UniformSampler sampler;

  auto t = p->intersect(ray);
  assert(t.is());
  Intersection isect(&s, 0, ray, t.get());

  uint valid_samples = 0;

  for (auto i = 0u; i < num_samples; ++i)
  {
    scalar p;
    spectrum reflectance;
    Vec3 d = mat->sample_bsdf(isect, -ray.direction.normal(), sampler, p, reflectance);
    if (p == 0)
      continue;
    r += reflectance / p * d.z;
    ++valid_samples;
  }

  return (r / scalar(num_samples)).luminance();
}

int main(int argc, char** args)
{
  using namespace refraction_index;

  //compare_bsdf(OrenNayar(1.0, 0.3), 100000000);
  //auto mat = make_shared<MirrorMaterial>();
  auto base_mat = make_shared<RoughColorMaterial>(0.0, spectrum(0.0));
  vector<scalar> angles({0.0, 5.0, 10.0, 20.0, 30.0, 45.0, 60.0, 80.0, 85.0, 89.0});

//  compare_tld(GTR(0.1), 10000000);
  return 0;
}
