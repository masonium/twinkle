#include "film.h"
#include "spectrum.h"
#include <algorithm>

using std::transform;

void CutoffToneMapper::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                               uint w, uint h) const
{
  transform(input.begin(), input.end(), std::back_inserter(output),
            [](const spectrum& s) { return s.clamp(0.0, 1.0); });
}

void LinearToneMapper::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                               uint w, uint h) const
{
  spectrum M = accumulate(input.begin(), input.end(),  spectrum::one, spectrum::max);
  scalar cM = max(max(M.x, M.y), M.z);

  transform(input.begin(), input.end(), std::back_inserter(output),
            [&](const spectrum& s) { return s / cM; });
}

void RSSFToneMapper::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                               uint w, uint h) const
{
  auto num_pixels = input.size();

  scalar delta = 0.001;
  vector<scalar> luminances(w*h);
  transform(input.begin(), input.end(), std::back_inserter(luminances),
            [](const spectrum& s) { return s.luminance(); });
  
  scalar total_log = accumulate(luminances.begin(), luminances.end(), scalar(0),
                                [=](scalar s, scalar v) { return s + log(delta + v); });
  
  scalar l_mult = middle_luminance * num_pixels / exp(total_log);

  scalar max_luminance = *max_element(luminances.begin(), luminances.end());
  scalar ml2 = max_luminance * max_luminance;
  
  transform(luminances.begin(), luminances.end(), luminances.begin(),
            [=](scalar s) { return s * (l_mult + s / ml2) / (1 + l_mult * s); });

  
}
