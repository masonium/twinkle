#pragma once

#include "math_util.h"
#include <random>

/*
 * Samplers return random numbers, with an amortized uniform distribution.
 */

template <int n>
struct Sample
{
  scalar u[n];
};

typedef Sample<2> Sample2D;
typedef Sample<4> Sample4D;

class Sampler1D
{
public:
  virtual scalar sample_1d() = 0;
};

class Sampler2D
{
public:
  virtual void sample_2d(scalar& r1, scalar& r2) = 0;
};

class Sampler4D
{
public:
  virtual void sample_4d(scalar& r1, scalar& r2, scalar& r3, scalar& r4) = 0;
};

class UniformSampler : public Sampler1D, public Sampler2D, public Sampler4D
{
public:
  UniformSampler();
  void seed(int seed_value);

  scalar sample_1d(void) override;
  Sample2D sample_2d()
  {
    Sample2D s;
    sample_2d(s.u[0], s.u[1]);
    return s;
  }

  void sample_2d(scalar& r1, scalar& r2);
  void sample_4d(scalar& r1, scalar& r2, scalar& r3, scalar& r4) override;

private:
  std::default_random_engine reng;
  std::uniform_real_distribution<scalar> randr;
};

class HaltonSampler : public Sampler1D, public Sampler2D, public Sampler4D
{
public:
  HaltonSampler();

  scalar sample_1d() override;
  void sample_2d(scalar& r1, scalar& r2) override;
  void sample_4d(scalar& r1, scalar& r2, scalar& r3, scalar& r4) override;

private:

  scalar sample_base(int i, int base, scalar binv);

  int64_t index;
  const int bases[4];
  const scalar inv_bases[4];
};