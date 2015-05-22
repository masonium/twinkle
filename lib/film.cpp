#include "film.h"
#include <numeric>
#include <iterator>
#include <algorithm>

using std::transform;
using std::back_inserter;
using std::copy;
using std::cout;
using std::ostream_iterator;
using std::accumulate;

Film::Film(uint w_, uint h_) : width(w_), height(h_), plate(w_ * h_)
{
}

void Film::add_sample(int x, int y, const spectrum& s)
{
  plate[index(x, y)].push_back(s);
}

void Film::render_to_ppm(ostream& out, ImageSampleFilter* filter, ToneMapper* mapper)
{
  vector<spectrum> comb;
  for (int y = height - 1; y >= 0; --y)
  {
    for (uint x = 0; x < width; ++x)
    {
      comb.push_back( filter->combine_samples(plate[index(x, y)]) );
    }
  }
  vector<spectrum> final;
  mapper->tonemap(comb, final, width, height);
  out << "P3 " << width << " " << height << " 255\n";

  for (auto& c: final)
  {
    out << int(c.x * 255) << " " << int(c.y * 255) << " " << int(c.z * 255) << " ";
  }
  
}

void Film::render_to_console(ostream& out)
{
  for (int y = height - 1; y >= 0; --y)
  {
    for (uint x = 0; x < width; ++x)
    {
      vector<spectrum>& pixel = plate[index(x, y)];
      spectrum s = pixel.empty() ? spectrum{0} : pixel[0];
      if (norm(s) > 0.5)
        out << '*';
      else if (norm(s) > 0.25)
        out << '.';
      else
        out << ' ';
    }
    out << '\n';
  }
}

////////////////////////////////////////////////////////////////////////////////

spectrum BoxFilter::combine_samples(const vector<spectrum>& samples) const
{
  if (samples.empty())
    return spectrum_zero;
  spectrum res{0};
  for (auto a: samples)
    res += a;

  auto r = res / scalar(samples.size());
  return r;
}

////////////////////////////////////////////////////////////////////////////////
void CutoffToneMapper::tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                               uint w, uint h) const
{
  transform(input.begin(), input.end(), std::back_inserter(output),
            [](const spectrum& s) { return s.clamp(0.0, 1.0); });
}
