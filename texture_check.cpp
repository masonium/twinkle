#include "texture.h"
#include "perlin.h"
#include "film.h"
#include "tonemap.h"
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

  auto filter = make_shared<BoxFilter>();
  Film f(width, height, filter.get());

  auto pn = PerlinNoise::get_instance();
  // auto eng = std::mt19937();
  // auto unf = std::uniform_real_distribution<scalar>(0, 256);

  Ray r(Vec3::zero, Vec3::zero);

  scalar freq = 2;

  scalar min = 5, max = -5;
  scalar sw = width, sh = height;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      //scalar z = 0.2;

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
    }
  }
  cerr << min << ", " << max << endl;
  f.render_to_ppm(cout, make_shared<CutoffToneMapper>());
}
