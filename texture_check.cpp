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

  scalar freq = 16;

  scalar min = 5, max = -5;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      scalar p = pn->noise(x * freq / width, y * freq / height );
      p += pn->noise(x * 2.0 * freq / width, y * 2.0 * freq / height ) * 0.5;
      p += pn->noise(x * 4.0 * freq / width, y * 4.0 * freq / height ) * 0.25;

      auto s = (p > 0) ? spectrum{1.0, 0.0, 0.0} : spectrum{0.0, 0.0, 1.0};

      f.add_sample(PixelSample(x, y, 0.5, 0.5, r), s);
      if (min > p)
        min = p;
      if (max < p)
        max = p;
    }
  }
  cerr << min << ", " << max << endl;
  f.render_to_ppm(cout, make_shared<CutoffToneMapper>());
}
