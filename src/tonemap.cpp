#include "film.h"
#include "spectrum.h"
#include <algorithm>
#include <iostream>

using std::transform;
using std::cerr;
using std::endl;

CompositeToneMapper::CompositeToneMapper(std::initializer_list<shared_ptr<ToneMapper>> lst)
  : mappers(lst)
{
  assert( mappers.size() > 0 );
}

void CompositeToneMapper::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                                  uint w, uint h) const
{
  mappers[0]->tonemap(input, output, w, h);

  for (size_t i = 1; i < mappers.size(); ++i)
  {
    mappers[i]->tonemap(output, output, w, h);
  }
}

void CutoffToneMapper::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                               uint w, uint h) const
{
  transform(input.begin(), input.end(), output.begin(),
            [](const spectrum& s) { return s.clamp(0.0, 1.0); });
}

void LinearToneMapper::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                               uint w, uint h) const
{
  spectrum M = accumulate(input.begin(), input.end(),  spectrum::one, spectrum::max);
  scalar cM = max(max(M.x, M.y), M.z);

  transform(input.begin(), input.end(), output.begin(),
            [&](const spectrum& s) { return s / cM; });
}

void RSSFToneMapper::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                               uint w, uint h) const
{
  auto num_pixels = input.size();

  scalar delta = 0.001;
  vector<scalar> luminances(w*h);
  transform(input.begin(), input.end(), luminances.begin(),
            [](const spectrum& s) { return s.luminance(); });
  
  scalar total_log = accumulate(luminances.begin(), luminances.end(), scalar(0),
                                [=](scalar s, scalar v) { return s + log(delta + v); });
  
  const scalar mean_log_luminance = exp(total_log / num_pixels);

  scalar l_mult = middle_luminance / mean_log_luminance;

  scalar max_luminance = *max_element(luminances.begin(), luminances.end());
  scalar ml2 = max_luminance * max_luminance;
/*  
  scalar mean_luminance = accumulate(luminances.begin(), luminances.end(), scalar(0)) / num_pixels;  cerr << "total_log: " << total_log << endl;
  cerr << "mean_log_luminance: " << mean_log_luminance << endl;
  cerr << "mean luminance: " << mean_luminance << endl;
  cerr << "l_mult: " << l_mult << endl;
  cerr << "max: " << max_luminance << endl;
  */
  transform(luminances.begin(), luminances.end(), luminances.begin(),
            [=](scalar s) { return s * (l_mult + s / ml2) / (1 + l_mult * s); });

  /*
  cerr << "transformed_max: " << *max_element(luminances.begin(), luminances.end()) << endl;
  scalar trans_mean_luminance = accumulate(luminances.begin(), luminances.end(), scalar(0)) / num_pixels;
  cerr << "transformed_mean: " << trans_mean_luminance << endl;
  */
  transform(input.begin(), input.end(), luminances.begin(), 
            output.begin(),
            [](const spectrum& sp, scalar s) { return sp.rescale_luminance(s); });
}
