#include "running_stats.h"
#include <cmath>

RunningStats::RunningStats()
  : mean_(0), ss_(0), N_(0)
{
}

void RunningStats::update(double sample)
{
  double old_mean = mean_;
  double next_N = N_ + 1;
  mean_ = mean_ + (sample - mean_) / next_N;
  ss_ += (mean_ - sample) * (old_mean - sample);
  N_ += 1;
}

double RunningStats::mean() const
{
  return mean_;
}

double RunningStats::variance() const
{
  return N_ > 1 ? ss_ / (N_ - 1) : 0;
}

double RunningStats::stdev() const
{
  return std::sqrt(variance());
}

int RunningStats::count() const
{
  return N_;
}
