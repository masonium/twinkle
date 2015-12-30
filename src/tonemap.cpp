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

sp_image CompositeToneMapper::tonemap(const sp_image& input) const
{
  auto a = mappers[0]->tonemap(input);

  for (size_t i = 1; i < mappers.size(); ++i)
  {
    a = mappers[i]->tonemap(a);
  }

  return a;
}

sp_image CutoffToneMapper::tonemap(const sp_image& input) const
{
  sp_image output(input.width(), input.height());
  transform(input.begin(), input.end(), output.begin(),
            [](const spectrum& s) { return s.clamp(0.0, 1.0); });
  return output;
}
sp_image LinearToneMapper::tonemap(const sp_image& input) const
{
  spectrum M = accumulate(input.begin(), input.end(), spectrum::one, spectrum::max);
  scalar cM = max(max(M.x, M.y), M.z);

  sp_image output(input.width(), input.height());
  transform(input.begin(), input.end(), output.begin(),
            [&](const spectrum& s) { return s / cM; });

  return output;
}
