#include "sampler.h"
#include "vec3.h"
#include "vec2.h"
#include "util.h"
#include <cmath>
#include <algorithm>
#include <stack>

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
  p = cosine_weighted_hemisphere_pdf(cwhs);
  return cwhs;
}

scalar cosine_weighted_hemisphere_pdf(const Vec3& v)
{
  return v.z * INV_PI;
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
  p = uniform_hemisphere_pdf(ret);
  return ret;
}

scalar uniform_hemisphere_pdf(const Vec3& UNUSED(v))
{
  return INV_2PI;
}

Vec3 uniform_sphere_sample(const Sample2D& sample)
{
  scalar theta = 2 * PI * sample[0];
  scalar phi = acos(sample[1] * 2.0 - 1.0);

  return Vec3::from_euler(theta, phi);
}

Vec3 uniform_sphere_sample(const Sample2D& sample, scalar& p)
{
  auto v = uniform_sphere_sample(sample);
  p = uniform_sphere_pdf(v);
  return v;
}
scalar uniform_sphere_pdf(const Vec3& UNUSED(v))
{
  return INV_4PI;
}

Vec2 uniform_sample_disc(const Sample2D& sample)
{
  scalar p;
  return uniform_sample_disc(sample, p);
}

Vec2 uniform_sample_disc(const Sample2D& sample, scalar& p)
{
  scalar x = sample[0] * 2.0 - 1.0;
  scalar y = sample[1] * 2.0 - 1.0;

  scalar r, theta;

  if (unlikely(x == 0 && y == 0))
    return Vec2(0, 0);

  if (x * x > y * y)
  {
    r = x;
    theta = PId4 * (y / x);
  }
  else
  {
    r = y;
    theta = PId2 - PId4 * (x / y);
  }

  p = 1 / (PI * PI);
  return Vec2(r * cos(theta), r * sin(theta));
}


////////////////////////////////////////////////////////////////////////////////
std::chrono::system_clock::duration::rep time_seed()
{
  return std::chrono::system_clock::now().time_since_epoch().count();
}


////////////////////////////////////////////////////////////////////////////////

// multinomial sampling via the alias method
using std::vector;
vector<uint> multinomial_distribution(const vector<double>& p_orig, uint64_t num_draws)
{
  using std::cerr;
  using std::endl;
  using std::ostream_iterator;

  UniformSampler s;
  s.seed(time_seed());

  const auto n = p_orig.size();
  vector<double> p(n);
  const double tp = accumulate(p_orig.begin(), p_orig.end(), 0.0);

  std::transform(p_orig.begin(), p_orig.end(), p.begin(),
                 [=] (double x) { return x / tp * n; });

  vector<int> alias(n);
  vector<double> prob(n);

  std::stack<uint> small, large;

  // Construct the tables.
  for (auto i = 0u; i < n; ++i)
  {
    if (p[i] < 1)
      small.push(i);
    else
      large.push(i);
  }

  while (!large.empty() && !small.empty())
  {
    auto g = large.top(), l = small.top();
    large.pop();
    small.pop();
    prob[l] = p[l];
    alias[l] = g;
    p[g] = (p[g] + p[l]) - 1.0;
    if (p[g] < 1)
      small.push(g);
    else
      large.push(g);
  }
  while (!large.empty())
  {
    prob[large.top()] = 1;
    large.pop();
  }
  while (!small.empty())
  {
    prob[small.top()] = 1;
    small.pop();
  }

  std::mt19937_64 eng;
  std::uniform_int_distribution<decltype(n)> slot_dist(0, n-1);
  std::uniform_real_distribution<double> alias_dist(0, 1.0);

  vector<uint> draws(n);

  // Execute the distribution;
  for (auto i = 0u; i < num_draws; ++i)
  {
    int slot = slot_dist(eng);
    if (alias_dist(eng) < prob[slot])
      ++draws[slot];
    else
      ++draws[alias[slot]];
  }

  return draws;
}
