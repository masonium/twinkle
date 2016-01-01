#include "test_util.h"
#include "spectrum.h"

namespace
{
  TEST(spectrum_rescale_luminance)
  {
    spectrum a{0.5, 0.5, 0.5};
    CHECK_CLOSE(0.5, a.luminance(), EPS);
    spectrum b = a.rescale_luminance(1.0);
    CHECK_CLOSE(1.0, b.luminance(), EPS);

    spectrum c{0.8, 1.2, 0.4};
    for (int i = 0; i < 20; ++i)
    {
      auto lum = rf() * 10.0;
      auto rs = c.rescale_luminance(lum);
      CHECK_CLOSE(lum, rs.luminance(), EPS);
      CHECK_CLOSE(2.0, rs[0]/rs[2], EPS);
      CHECK_CLOSE(3.0, rs[1]/rs[2], EPS);
    }
  }
}
