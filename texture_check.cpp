#include "texture.h"
#include "perlin.h"
#include "film.h"
#include "tonemap.h"
#include "reinhard.h"
#include "textures/skytexture.h"
#include <random>

using std::make_shared;
using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char** args)
{
  assert(argc > 2);

  int width = atoi(args[1]);
  int height = atoi(args[2]);
  int angle = atoi(args[3]);

  auto filter = make_shared<BoxFilter>();
  Film f(width, height, filter);

  auto pn = PerlinNoise::get_instance();
  auto sky = ShirleySkyTexture(Vec3::from_euler(3*PI/4, angle * PI / 180), 8);
  // auto eng = std::mt19937();
  // auto unf = std::uniform_real_distribution<scalar>(0, 256);

  Ray r(Vec3::zero, Vec3::zero);

  scalar min = 5, max = -5;
  scalar sw = width, sh = height;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      //scalar z = 0.2;
      auto uv = Vec2(x / sw, 0.5 - (y / sh) * 0.5);

      auto s = sky.at_coord(uv);
      f.add_sample(PixelSample(x, y, 0.5, 0.5, r), s);

/*
      scalar p = unitize(sym_fbm_2d(pn, x / sw, y / sh, freq, 4, 3.0, 0.75));

      //scalar p = unitize(pn->noise(x / sw * freq, y / sh * freq));
      spectrum s;
      auto mapper = [](scalar x) { return x; };
      spectrum c2{0.2};
      if (p < 0.5)
        s = lerp(spectrum{0.8, 0.2, 0.2}, c2, mapper(p*2.0));
      else
        s = lerp(c2, spectrum{0.2, 0.1, 0.9}, mapper((p-0.5)*2.0));

      f.add_sample(PixelSample(x, y, 0.5, 0.5, r), s);//spectrum{static_cast<scalar>((p + 1.0) * 0.5)});
      if (min > p)
        min = p;
      if (max < p)
        max = p;
*/
      scalar p = s.luminance();
      if (min > p)
        min = p;
      if (max < p)
        max = p;
    }
  }
  cerr << min << ", " << max << endl;
  auto mapper = ReinhardLocal{ReinhardLocal::Options()};
  f.render_to_ppm(cout, mapper);
}
