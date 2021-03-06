#include <numeric>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <mutex>
#include <cassert>
#include "util.h"
#include "film.h"
#include "sampler.h"
#include "reinhard.h"

using std::transform;
using std::back_inserter;
using std::copy;
using std::cout;
using std::ostream_iterator;
using std::accumulate;
using std::cerr;
using std::endl;

Film::Film(uint w_, uint h_)
  : width(w_), height(h_), plate(w_ * h_)
{
}

Film::Film(const Film& f)
  : width(f.width), height(f.height), plate(f.plate)
{
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
    plate[i] = AccPixel{ spectrum::deserialize(in) };
  }
}

void Film::add_sample(const PixelSample& ps, const spectrum& s, scalar w)
{
  BoxFilter().add_sample(*this, ps, s, w);
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

sp_image Film::image() const
{
  sp_image img(width, height);
  
  transform(plate.begin(), plate.end(), img.begin(),
            [](const auto& p) { return p.value(); });
            
  return img;
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

sp_image Film::as_weights() const
{
  sp_image f(this->width, this->height);
  transform(plate.begin(), plate.end(),
            f.begin(),
            [] (const auto& pixel)
            {
              return spectrum{pixel.weight};
            });

  return f;
}

sp_image Film::as_pv() const
{
  sp_image f(this->width, this->height);
  transform(plate.begin(), plate.end(),
            f.begin(),
            [] (const auto& pixel)
            {
              return spectrum{pixel.perceptual_variance()};
            });

  return f;
}

Array2D<uint> Film::samples_by_variance(uint spp) const
{
  uint64_t total_samples = spp * width * height;

  // FIX: This number should be dependent on spp.
  const double epsilon = 0.001;

  vector<double> weights(plate.size());

  // The weights are proportional to the (tonemapped) perceptual variances, with
  // a small epsilon to ensure a minimum number of samples for every pixel.
  {
    vector<spectrum> variances(plate.size());
    transform(plate.begin(), plate.end(), variances.begin(),
              [] (const auto& pixel) { return spectrum(pixel.perceptual_variance()); });

    vector<spectrum> ov(plate.size());
    //ReinhardGlobal().tonemap(variances, ov, width, height);
    ov = variances;
    
    double tv = accumulate(ov.begin(), ov.end(), 0.0, 
                           [](double v, const spectrum& s) { return v + s.x; });
    //cerr << "Average variance: " << tv / (width * height) << "\n";

    transform(ov.begin(), ov.end(), weights.begin(),
              [=] (const auto& v) { return epsilon + (1-epsilon) * v.x / tv; });

    scalar tw = accumulate(weights.begin(), weights.end(), 0.0);

    transform(weights.begin(), weights.end(), weights.begin(),
              [=] (const auto& w) { return w / tw * total_samples; });
  }

  vector<uint> samples(plate.size(), 0);
  int64_t remaining_samples = total_samples;
  for (uint i = 0u; i < samples.size(); ++i)
  {
    scalar f = std::floor(weights[i]);
    samples[i] = f;
    remaining_samples -= samples[i];
    weights[i] -= f;
  }

  // Get the samples from the residual distribution
  auto residual_samples = multinomial_distribution(weights, remaining_samples);

  transform(samples.begin(), samples.end(), residual_samples.begin(), samples.begin(),
            [](auto s, auto r) { return s + r; });

  return Array2D<uint>(width, height, samples);
}

void Film::clear()
{
  fill(plate.begin(), plate.end(), AccPixel());
}


////////////////////////////////////////////////////////////////////////////////

void BoxFilter::add_sample(Film& film, const PixelSample& p, 
                           const spectrum& s, scalar w) const
{
  Film::AccPixel& fp = film.at(p.x, p.y);
  fp.add_sample(s, w);
}

Film::AccPixel::AccPixel()
  : weight(0), total(0.0), mean(0), ss(0), count(0)
{

}

Film::AccPixel& Film::AccPixel::operator+=(const AccPixel& p)
{
  const scalar tw = weight + p.weight;

  if (p.weight == 0)
    return *this;

  const scalar delta = (p.mean - mean);

  mean +=  delta * p.weight / tw;
  ss += p.ss + delta * delta * weight * p.weight / tw;
  count += p.count;
  total += p.total;
  weight = tw;

  return *this;
}

void Film::AccPixel::add_sample(const spectrum& v, scalar w)
{
  if (unlikely(w == 0))
    return;

  total += v * w;
  scalar tw = weight + w;

  const scalar c = v.luminance();
  const scalar d = c - mean;
  const scalar R = d * w / tw;
  mean += R;
  ss += weight * d * R;
  count += 1;
  weight = tw;
  
  if (isnan(mean))
  {
    cerr << w << " " << c << " " << d << " " << tw << " " << R << "\n";
  }

  assert(!isnan(mean));
//  assert(!isnan(ss));
}

scalar Film::AccPixel::variance() const
{
  if (unlikely(weight <= 0))
    return 0;

  return (ss / weight);
}

scalar Film::AccPixel::perceptual_variance() const
{
  if (unlikely(weight <= 0))
    return 0;

  return (ss / weight) / tvi(mean);
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

