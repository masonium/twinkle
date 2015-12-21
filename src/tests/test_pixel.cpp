#include "test_util.h"
#include "film.h"

namespace
{
  TEST(variance_samples)
  {
    Film::AccPixel p;
    for (int i = 1; i <= 20; ++i)
    {
      p.add_sample(spectrum(i), 1.0);
      CHECK_CLOSE((square(i - 1 + 1) - 1) / 12.0, p.variance(), EPS);
    }
  }

  TEST(variance_weighted)
  {
    for (int i = 1; i <= 20; ++i)
    {
      Film::AccPixel p1, p2;
      for (int j = 1; j <= i; ++j)
      {
        p1.add_sample(spectrum(j), j);
        for (int k = 0; k < j; ++k)
          p2.add_sample(spectrum(j), 1.0);
        CHECK_CLOSE(p2.variance(), p1.variance(), EPS);
      }
    }

  }

  TEST(variance_pixels)
  {
    Film::AccPixel p1, p2;
    for (int i = 1; i <= 10; ++i)
    {
      p1.add_sample(spectrum(i), 1.0);
      p2.add_sample(spectrum(i+10), 1.0);
    }
    CHECK_CLOSE((square(20) - 1) / 12.0, (p1+p2).variance(), EPS);
  }

};
