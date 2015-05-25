#include "sampler.h"

UniformSampler::UniformSampler() : randr(0.0, 1.0)
{
}

void UniformSampler::seed(int seed_value)
{
  reng.seed(seed_value);
}

scalar UniformSampler::sample_1d()
{
  return randr(reng);
}

void UniformSampler::sample_2d(scalar& r1, scalar& r2)
{
  r1 = randr(reng);
  r2 = randr(reng);
}

void UniformSampler::sample_4d(scalar& r1, scalar& r2, scalar& r3, scalar& r4)
{
  r1 = randr(reng);
  r2 = randr(reng);
  r3 = randr(reng);
  r4 = randr(reng);
}

////////////////////////////////////////////////////////////////////////////////
HaltonSampler::HaltonSampler() : index(1), bases{2, 3, 5, 7},
                                 inv_bases{1/2.0, 1/3.0, 1/5.0, 1/7.0}
{

}

scalar HaltonSampler::sample_base(int i, int base, scalar invb)
{
  scalar r = 0;
  scalar invbp = invb;
  while (i > 0)
  {
    int j = i % base;
    r += j * invbp;
    invbp *= invb;
    i /= base;
  }
  return r;
}

scalar HaltonSampler::sample_1d()
{
  scalar r1 = sample_base(index, bases[0], inv_bases[0]);
  index += 1;
  return r1;
}
void HaltonSampler::sample_2d(scalar& r0, scalar& r1)
{
  r0 = sample_base(index, bases[0], inv_bases[0]);
  r1 = sample_base(index, bases[1], inv_bases[1]);
  index += 1;
}
void HaltonSampler::sample_4d(scalar& r0, scalar& r1, scalar& r2, scalar& r3)
{
  r0 = sample_base(index, bases[0], inv_bases[0]);
  r1 = sample_base(index, bases[1], inv_bases[1]);
  r2 = sample_base(index, bases[2], inv_bases[2]);
  r3 = sample_base(index, bases[3], inv_bases[3]);
  index += 1;
}
