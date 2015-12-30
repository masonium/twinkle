#include <algorithm>
#include <iostream>
#include <cassert>
#include "reinhard.h"

using std::transform;

/*
 * Compute the luminance vector Lw, as well as the mean log luminance, as
 * specified in Eq. 1.
 */
void reinhard_tonemap_aux(const sp_image& input,
                          vector<scalar>& luminances, scalar& mean_log_luminance)
{
  scalar delta = 0.001;
  const auto n = input.size();
  luminances.resize(n);

  transform(input.begin(), input.end(), luminances.begin(),
            [](const spectrum& s) { return s.luminance(); });

  scalar total_log = accumulate(luminances.begin(), luminances.end(), scalar(0),
                                [=](scalar s, scalar v) { return s + log(delta + v); });

  mean_log_luminance = exp(total_log / n);
}

sp_image ReinhardGlobal::tonemap(const sp_image& input) const
{
  const auto w = input.width(), h = input.height();
  vector<scalar> luminances(w*h);
  scalar mean_log_luminance;
  reinhard_tonemap_aux(input, luminances, mean_log_luminance);

  scalar l_mult = key_value / mean_log_luminance;
  scalar max_luminance = *max_element(luminances.begin(), luminances.end());
  scalar ml2 = max_luminance * max_luminance;

  // Instead of compute Eq. 2 and Eq. 4 separately, we merge them into the same
  // transform calcuation.
  transform(luminances.begin(), luminances.end(), luminances.begin(),
            [=](scalar s) { return s * (l_mult + s / ml2) / (1 + l_mult * s); });

  sp_image output(w, h);
  transform(input.begin(), input.end(), luminances.begin(),
            output.begin(),
            [](const spectrum& sp, scalar s) { return sp.rescale_luminance(s).clamp(); });

  return output;
}

ReinhardLocal::ReinhardLocal(const Options& options) : opt(options)
{
}

ReinhardLocal::Options::Options()
  : key_value(0.18), scale_selection_threshhold(0.05), sharpening_factor(8),
    scale_pixel_factor(sqrt(0.125)), cs_ratio(1.6), num_scales(8)
{
}

sp_image ReinhardLocal::tonemap(const sp_image& input) const
{
  const auto w = input.width(), h = input.height();
  vector<scalar> luminances(w*h);
  scalar mean_log_luminance;
  reinhard_tonemap_aux(input, luminances, mean_log_luminance);
  scalar l_mult = opt.key_value / mean_log_luminance;

  // Explicitly compute L(x, y) as specified in equation 2.
  transform(luminances.begin(), luminances.end(), luminances.begin(),
            [=](scalar L) { return L * l_mult; });

  auto center_surround = center_surround_functions(w, h, luminances, opt);

  vector<scalar> display_lum(input.size());
  for (auto i = 0u; i < luminances.size(); ++i)
  {
    int scale = 0;
    for ( ; scale < opt.num_scales - 1; ++scale)
    {
      if (fabs(center_surround[scale][i].first) < opt.scale_selection_threshhold)
        break;
    }
    display_lum[i] = luminances[i] / (1 + center_surround[scale][i].second);
    //display_lum[i] = center_surround[5][i].first;
  }

  sp_image output(w, h);
  transform(display_lum.begin(), display_lum.end(), input.begin(), output.begin(),
            [](scalar lum, const spectrum& s) { return s.rescale_luminance(std::min<scalar>(lum, 1.0)).clamp(); });
  // transform(display_lum.begin(), display_lum.end(), output.begin(),
  //           [](scalar lum)
  //           {
  //             return spectrum{lum};
  //           });

  return CutoffToneMapper().tonemap(output);
}

vector<vector<pair<scalar, scalar>>> ReinhardLocal::center_surround_functions(
  uint w, uint h, const vector<scalar>& luminances, const Options& opt)
{
  vector<vector<pair<scalar, scalar>>> cs(opt.num_scales);

  vector<scalar> v1, v2, filter;
  scalar current_scale = 1;

  // compute the first center scale
  {
    scalar v1_gf_sigma = opt.scale_pixel_factor * current_scale;
    int fs = gaussian_filter(v1_gf_sigma, filter);
    convolve(w, h, luminances, fs, fs, filter, v1);
  }

  scalar normalizing_mult = pow(2, opt.sharpening_factor) * opt.key_value;

  /*
   * At each iteration, compute the surround function, followed by the
   * center-surround function.
   */
  for (int i = 0; i < opt.num_scales; ++i)
  {
    scalar v2_gf_sigma = opt.scale_pixel_factor * current_scale * opt.cs_ratio;
    int fs = gaussian_filter(v2_gf_sigma, filter);
    const scalar normalization = normalizing_mult / (current_scale * current_scale);
    convolve(w, h, luminances, fs, fs, filter, v2);

    cs[i].resize(v1.size());
    transform(v1.begin(), v1.end(), v2.begin(), cs[i].begin(),
              [=](scalar r1, scalar r2)
              {
                return std::make_pair((r1 - r2) / (normalization + r1), r1);
              });

    // We increase the scale by the same factor as the center-surround ratio, so
    // that we can reuse the surround from the previous scale as the center for
    // the current scale.
    current_scale *= opt.cs_ratio;
    std::swap(v1, v2);
  }

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
