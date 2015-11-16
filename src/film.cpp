#include <numeric>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <mutex>
#include <cassert>
#include "film.h"

using std::transform;
using std::back_inserter;
using std::copy;
using std::cout;
using std::ostream_iterator;
using std::accumulate;
using std::cerr;

Film::Film(uint w_, uint h_)
  : width(w_), height(h_), plate(w_ * h_)
{
}

Film::Film(const Film& f)
  : width(f.width), height(f.height), plate(f.plate)
{
}

Film& Film::operator=(const Film& f)
{
  width = f.width;
  height = f.height;
  plate = f.plate;
  return *this;
}

Film& Film::operator=(Film&& f)
{
  width = f.width;
  height = f.height;
  swap(plate, f.plate);
  return *this;
}

Film::Film(istream& in) : width(0), height(0)
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
    plate[i] = AccPixel{ scalar(1.0), spectrum::deserialize(in) };
  }
}

void Film::add_sample(const PixelSample& ps, const spectrum& s)
{
  BoxFilter().add_sample(*this, ps, s);
}

vector<spectrum> Film::pixel_list() const
{
  vector<spectrum> ret;
  for (const auto& p: plate)
  {
    assert(p.weight > 0);
    ret.push_back( p.total / p.weight );
  }
  return ret;
}

void Film::merge(const Film& f) 
{
  assert(width == f.width);
  assert(height == f.height);
  assert(plate.size() == f.plate.size());

  transform(plate.begin(), plate.end(), f.plate.begin(),
            plate.begin(), [](auto& x, const auto& y) { return x + y; });
}

scalar Film::average_intensity() const
{
  scalar r = 0, w = 0;
  for (const auto& p: plate)
  {
    w += p.weight;
    r += (p.total.luminance() - r) / w; 
  }

  return r;
}


void Film::render_to_ppm(ostream& out, const ToneMapper& mapper)
{
  vector<spectrum> final = pixel_list();
  mapper.tonemap(final, final, width, height);
  out << "P3 " << width << " " << height << " 255\n";

  for (int y = height - 1; y >= 0; --y)
  {
    for(uint x = 0; x < width; ++x)
    {
      const auto& c = final[index(x, y)];//.clamp(0, 1);
      assert(c.x >= 0);
      assert(c.y >= 0);
      assert(c.z >= 0);

      assert(c.x <= 1);
      assert(c.y <= 1);
      assert(c.z <= 1);
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

void Film::clear()
{
  fill(plate.begin(), plate.end(), AccPixel());
}


////////////////////////////////////////////////////////////////////////////////

void BoxFilter::add_sample(Film& film, const PixelSample& p, const spectrum& s) const
{
  Film::AccPixel& fp = film.at(p.x, p.y);
  fp.total += s;
  fp.weight += 1;
}

Film::AccPixel& Film::AccPixel::operator+=(const AccPixel& p)
{
  total += p.total;
  weight += p.weight;
  return *this;
}
Film::AccPixel Film::AccPixel::operator+(const AccPixel& p) const
{
  AccPixel x(*this);
  return x += p;
}

////////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& o, const Film::AccPixel& pixel)
{
  o << "Rect(" << pixel.total << ", " << pixel.weight << ")";
  return o;
}

ostream& operator<<(ostream& o, const Film::Rect& rect)
{
  o << "Rect(" << rect.x << ", " << rect.y << "; " << rect.width << ", " << rect.height << ")";
  return o;
}

