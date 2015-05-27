
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

Film::Film(uint w_, uint h_, ImageSampleFilter* f) : width(w_), height(h_), plate(w_ * h_), filter(f)
{
}

void Film::add_sample(const PixelSample& ps, const spectrum& s)
{
  filter->add_sample(this, ps, s);
}

vector<spectrum> Film::pixel_list() const
{
  vector<spectrum> ret;
  for (const auto& p: plate)
    ret.push_back( p.total / p.weight );

  return ret;
}

void Film::render_to_ppm(ostream& out, ToneMapper* mapper)
{
  vector<spectrum> final;
  vector<spectrum> raw = pixel_list();
  mapper->tonemap(raw, final, width, height);
  out << "P3 " << width << " " << height << " 255\n";

  for (int y = height - 1; y >= 0; --y)
  {
    for(uint x = 0; x < width; ++x)
    {
      const auto& c = final[index(x, y)];
      out << int(c.x * 255) << " " << int(c.y * 255) << " " << int(c.z * 255) << " ";
    }
    out << "\n";  
  }
}

void Film::render_to_console(ostream& out)
{
  for (int y = height - 1; y >= 0; --y)
  {
    for (uint x = 0; x < width; ++x)
    {
      spectrum s = plate[index(x,y)].total;
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

void BoxFilter::add_sample(Film* film, const PixelSample& p, const spectrum& s) const
{
  FilmPixel& fp = film->at(p.x, p.y);
  fp.total += s;
  fp.weight += 1;
}

////////////////////////////////////////////////////////////////////////////////
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
