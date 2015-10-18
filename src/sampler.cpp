#include "sampler.h"
#include "vec3.h"
#include <cmath>

using std::max;

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

void UniformSampler::sample_5d(scalar& r1, scalar& r2, scalar& r3, scalar& r4, scalar& r5)
{
  r1 = randr(reng);
  r2 = randr(reng);
  r3 = randr(reng);
  r4 = randr(reng);
  r5 = randr(reng);
}

////////////////////////////////////////////////////////////////////////////////
ConstSampler::ConstSampler(scalar a, scalar b, scalar c, scalar d, scalar e) :
  base{a, b, c, d, e}
{
}

scalar ConstSampler::sample_1d()
{
  return base[0];
}

void ConstSampler::sample_2d(scalar& r1, scalar& r2)
{
  r1 = base[0];
  r2 = base[1];
}

void ConstSampler::sample_4d(scalar& r1, scalar& r2, scalar& r3, scalar& r4)
{
  r1 = base[0];
  r2 = base[1];
  r3 = base[2];
  r4 = base[3];
}

void ConstSampler::sample_5d(scalar& r1, scalar& r2, scalar& r3, scalar& r4, scalar& r5)
{
  r1 = base[0];
  r2 = base[1];
  r3 = base[2];
  r4 = base[3];
  r5 = base[4];
}

////////////////////////////////////////////////////////////////////////////////
HaltonSampler::HaltonSampler() : index(1), bases{2, 3, 5, 7, 11},
                                 inv_bases{1/2.0, 1/3.0, 1/5.0, 1/7.0, 1/11.0}
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
void HaltonSampler::sample_5d(scalar& r0, scalar& r1, scalar& r2, scalar& r3, scalar& r4)
{
  r0 = sample_base(index, bases[0], inv_bases[0]);
  r1 = sample_base(index, bases[1], inv_bases[1]);
  r2 = sample_base(index, bases[2], inv_bases[2]);
  r3 = sample_base(index, bases[3], inv_bases[3]);
  r4 = sample_base(index, bases[4], inv_bases[4]);
  index += 1;
}

////////////////////////////////////////////////////////////////////////////////

Vec3 cosine_weighted_hemisphere_sample(const Sample2D& sample)
{
  const scalar theta = 2 * PI * sample[0];
  const scalar radius = sqrt(sample[1]);

  const scalar x = radius * cos(theta), y = radius * sin(theta);
  const scalar z = sqrt(max(scalar(0.0), 1 - sample[1]));
  
  return Vec3{x, y, z};
}

Vec3 cosine_weighted_hemisphere_sample(const Sample2D& sample, scalar& p)
{
  auto cwhs = cosine_weighted_hemisphere_sample(sample);
  p = sin(2 * cwhs.z) * INV_2PI;
  return cwhs;
}

Vec3 uniform_hemisphere_sample(const Sample2D& sample)
{
  scalar theta = 2 * PI * sample[0];
  scalar phi = acos(sample[1]);

  return Vec3::from_euler(theta, phi);
}

Vec3 uniform_hemisphere_sample(const Sample2D& sample, scalar& p)
{
  auto ret = uniform_hemisphere_sample(sample);
  p = INV_2PI;
  return ret;
}

Vec3 uniform_sphere_sample(const Sample2D& sample)
{
  scalar theta = 2 * PI * sample[0];
  scalar phi = acos(sample[1] * 2.0 - 1.0);

  return Vec3::from_euler(theta, phi);
}
