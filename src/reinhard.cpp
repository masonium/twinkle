#include <algorithm>
#include <iostream>
#include <cassert>
#include "reinhard.h"

using std::transform;

/*
 * Compute the luminance vector Lw, as well as the mean log luminance, as
 * specified in Eq. 1.
 */
void reinhard_tonemap_aux(const vector<spectrum>& input, int n,
                          vector<scalar>& luminances, scalar& mean_log_luminance)
{
  scalar delta = 0.001;
  luminances.resize(n);

  transform(input.begin(), input.end(), luminances.begin(),
            [](const spectrum& s) { return s.luminance(); });

  scalar total_log = accumulate(luminances.begin(), luminances.end(), scalar(0),
                                [=](scalar s, scalar v) { return s + log(delta + v); });

  mean_log_luminance = exp(total_log / n);
}


void ReinhardGlobal::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                             uint w, uint h) const
{
  vector<scalar> luminances(w*h);
  scalar mean_log_luminance;
  reinhard_tonemap_aux(input, w*h, luminances, mean_log_luminance);

  scalar l_mult = key_value / mean_log_luminance;
  scalar max_luminance = *max_element(luminances.begin(), luminances.end());
  scalar ml2 = max_luminance * max_luminance;

  // Instead of compute Eq. 2 and Eq. 4 separately, we merge them into the same
  // transform calcuation.
  transform(luminances.begin(), luminances.end(), luminances.begin(),
            [=](scalar s) { return s * (l_mult + s / ml2) / (1 + l_mult * s); });

  transform(input.begin(), input.end(), luminances.begin(),
            output.begin(),
            [](const spectrum& sp, scalar s) { return sp.rescale_luminance(s).clamp(); });
}

ReinhardLocal::ReinhardLocal(scalar mid, scalar center_surround)
  : key_value(mid), scale_param(0.5 / sqrt(2)), cs_ratio(center_surround),
    num_scales(8)
{
}

void ReinhardLocal::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                            uint w, uint h) const
{
  vector<scalar> luminances(w*h);
  scalar mean_log_luminance;
  reinhard_tonemap_aux(input, w*h, luminances, mean_log_luminance);
  scalar l_mult = key_value / mean_log_luminance;

  // Explicitly compute L(x, y) as specified in equation 2.
  transform(luminances.begin(), luminances.end(), luminances.begin(),
            [=](scalar L) { return L * l_mult; });

  auto center_surround = center_surround_functions(luminances, w, h, num_scales);
}

vector<vector<scalar>> ReinhardLocal::center_surround_functions(
  const vector<scalar>& luminances, uint w, uint h, int num_scales)
{
  vector<vector<scalar>> cs(num_scales);

  vector<scalar> v1, v2, filter;
  scalar current_scale = 1;

  int filter_size = gaussian_filter(current_scale, filter);
  convolve(w, h, luminances, filter_size, filter_size, filter, v1);

  return cs;
}

/*
 * Returns the gaussian filter with sigma scale parameter.
 *
 * The input vector is resized to fit the filter, and the size of the filter (on
 * a side) is returned.
 */
int ReinhardLocal::gaussian_filter(scalar scale, vector<scalar>& filter)
{
  int hf_size = ceil(scale);
  int filter_size = hf_size * 2 + 1;

  filter.resize(0);
  filter.reserve(filter_size * filter_size);

  const scalar s2 = scale * scale;
  for (int y = -hf_size; y <= hf_size; ++y)
  {
    for (int x = -hf_size; x <= hf_size; ++x)
    {
      filter.push_back( exp( -(x*x + y*y) / s2) / (PI * s2) );
    }
  }

  return filter_size;
}

/**
 * Integer square root algorithm, taken from:
 * http://www.codecodex.com/wiki/Calculate_an_integer_square_root
 */
unsigned int isqrt(unsigned long n)
{
  unsigned int c = 0x8000;
  unsigned int g = 0x8000;

  while (true) {
    if (g * g > n)
      g ^= c;
    c >>= 1;
    if (c == 0)
      return g;
    g |= c;
  }
}
