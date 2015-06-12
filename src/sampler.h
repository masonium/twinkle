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
  scalar operator[](int i) const
  {
    return u[i];
  }
  scalar& operator[](int i)
  {
    return u[i];
  }
};

typedef Sample<2> Sample2D;
typedef Sample<4> Sample4D;
typedef Sample<5> Sample5D;

class Sampler
{
public:
  virtual scalar sample_1d() = 0;
  virtual void sample_2d(scalar& r1, scalar& r2) = 0;
  virtual void sample_4d(scalar& r1, scalar& r2, scalar& r3, scalar& r4) = 0;
  virtual void sample_5d(scalar& r1, scalar& r2, scalar& r3, scalar& r4, scalar& r5) = 0;
  Sample5D sample_5d()
  {
    Sample5D s;
    sample_5d(s[0], s[1], s[2], s[3], s[4]);
    return s;
  }

  virtual ~Sampler() { }
};

class UniformSampler : public Sampler
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
  
  using Sampler::sample_5d;
  void sample_5d(scalar& r1, scalar& r2, scalar& r3, scalar& r4, scalar& r5) override;

private:
  std::default_random_engine reng;
  std::uniform_real_distribution<scalar> randr;
};

class HaltonSampler : public Sampler
{
public:
  HaltonSampler();

  scalar sample_1d() override;
  void sample_2d(scalar& r1, scalar& r2) override;
  void sample_4d(scalar& r1, scalar& r2, scalar& r3, scalar& r4) override;

  using Sampler::sample_5d;
  void sample_5d(scalar& r1, scalar& r2, scalar& r3, scalar& r4, scalar& r5) override;
private:

  scalar sample_base(int i, int base, scalar binv);

  int64_t index;
  const int bases[5];
  const scalar inv_bases[5];
};

Vec3 cosine_weighted_hemisphere_sample(const Sample2D& sample);
Vec3 uniform_hemisphere_sample(const Sample2D& sample);
