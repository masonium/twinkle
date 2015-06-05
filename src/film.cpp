#include <numeric>
#include <iterator>
#include <algorithm>
#include <cstring>
#include "film.h"

using std::transform;
using std::back_inserter;
using std::copy;
using std::cout;
using std::ostream_iterator;
using std::accumulate;
using std::cerr;

Film::Film(uint w_, uint h_, const ImageSampleFilter* f)
  : width(w_), height(h_), filter(f), plate(w_ * h_)
{
}


Film::Film(istream& in) : width(0), height(0), filter(new BoxFilter)
{
  char buf[8];
  in.read(buf, 8);
  if (strncmp(buf, "TWINKLE", 8) != 0)
    return;

  in.read(reinterpret_cast<char*>(const_cast<uint32_t*>(&width)), sizeof(width));
  in.read(reinterpret_cast<char*>(const_cast<uint32_t*>(&height)), sizeof(height));

  if (!in.good())
    return;
  if (width == 0 || height == 0)
    return;

  plate.resize(width*height);
  for (uint32_t i = 0; i < width * height; ++i)
  {
    plate[i] = Pixel{ scalar(1.0), spectrum::deserialize(in) };
  }
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

void Film::merge(const Film& f) 
{
  transform( plate.begin(), plate.end(),
             f.plate.begin(),
             plate.begin(),
             [](Pixel a, Pixel b) { return Pixel{a.weight + b.weight, a.total + b.total}; });
}

void Film::render_to_ppm(ostream& out, weak_ptr<ToneMapper> mapper)
{
  vector<spectrum> final = pixel_list();
  mapper.lock()->tonemap(final, final, width, height);
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

void Film::render_to_twi(ostream& out) const
{
  vector<spectrum> raw = pixel_list();
  out.write("TWINKLE", 8);
  out.write(reinterpret_cast<const char*>(&width), sizeof(width));
  out.write(reinterpret_cast<const char*>(&height), sizeof(height));
  for (auto& s: raw)
    s.serialize(out);
}

void Film::render_to_console(ostream& out) const
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
  Film::Pixel& fp = film->at(p.x, p.y);
  fp.total += s;
  fp.weight += 1;
}
