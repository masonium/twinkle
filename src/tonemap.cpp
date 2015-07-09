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
