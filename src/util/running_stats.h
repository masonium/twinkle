#pragma once

class RunningStats
{
public:
  RunningStats();

  void update(double sample);

  double mean() const;
  double variance() const;
  double stdev() const;

  int count() const;

private:
  double mean_;
  double ss_;
  int N_;
};
