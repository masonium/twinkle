#include "tests/test_util.h"
#include "util/running_stats.h"

namespace
{	
  SUITE(RunningStats)
  {
    TEST(uniform)
    {
      RunningStats stats;
      for (int i = 1; i <= 5; ++i)
      {
        stats.update(i);
        CHECK_CLOSE((1.0 + i) * 0.5, stats.mean(), PRECISE_EPS);
        CHECK_EQUAL(i, stats.count());
        double sample_bias = i > 1 ? double(i)/(i-1) : 0;
        double var = sample_bias * ((i+1)*(2*i+1) / 6.0  - (i+1)*(i+1) / 2.0 + (i+1)*(i+1)/4.0);
        CHECK_CLOSE(var, stats.variance(),PRECISE_EPS);
      }
    }
  }
}

