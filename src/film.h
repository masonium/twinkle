#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <atomic>
#include "spectrum.h"
#include "ray.h"
#include "tonemap.h"
#include "util/array.h"

using std::atomic;
using std::vector;
using std::ostream;
using std::weak_ptr;

class Film;

struct PixelSample
{
  PixelSample() = default;

  PixelSample(int x_, int y_, scalar px_, scalar py_, Ray ray_) :
    x(x_), y(y_), px(px_), py(py_), ray(ray_)
  {
  }

  int x, y;
  scalar px, py; // (0.5, 0.5) is the center of the pixel.
  Ray ray;
};

using PixelSampleValue = pair<PixelSample, spectrum>;
using SampleVector = vector<PixelSampleValue>;

/**
 * An image sample filter combines adds a pixel sample to the film. A given
 * pixel sample is allowed to contribute to multiple pixels, but the filter must
 * be linear.
 */
class ImageSampleFilter
{
public:
  virtual void add_sample(Film& film, const PixelSample& p, const spectrum& s, scalar w) const = 0;

  virtual ~ImageSampleFilter() { }
};

/**
 * In a BoxFilter, each pixel sample contributes only to its native pixel. All
 * samples within the pixel are counted equally.
 */
class BoxFilter : public ImageSampleFilter
{
public:
  void add_sample(Film& film, const PixelSample& p, 
                  const spectrum& s, scalar w = 1.0) const override;
};


class Film
{
public:
  struct Rect
  {
    Rect(uint x_, uint y_, uint w_, uint h_) : x(x_), y(y_), width(w_), height(h_) {} 
    Rect() : Rect(0, 0, 0, 0) {}
    
    uint x, y;
    uint width, height;
  };

  struct AccPixel  // short for AccumulationPixel
  {
    AccPixel();
    AccPixel(const spectrum& t, scalar w = 1.0) : 
      weight(w), total(t), mean(t.luminance()), ss(0), count(1) { }

    AccPixel& operator +=(const AccPixel&);
    AccPixel operator +(const AccPixel&) const;

    void add_sample(const spectrum& v, scalar w);

    scalar variance() const;
    scalar perceptual_variance() const;

    spectrum value() const { return total / weight; }

    scalar weight;
    spectrum total;
    scalar mean;
    scalar ss;
    uint count;
  };
  
  Film(uint w_, uint h_);
  Film(istream& in);

  Film(const Film& f);

  Array2D<uint> samples_by_variance(uint spp) const;

  Film as_weights() const;
  Film as_pv() const;

  void add_sample(const PixelSample& ps, const spectrum& s, scalar w = 1.0);

  /**
   * Return the (weighted) average instensity over image region.
   */
  scalar average_intensity() const;

  Rect rect() const { return Rect(0, 0, width, height); }

  void render_to_console(ostream& out) const;
  void render_to_ppm(ostream& out, const ToneMapper& mapper);
  void render_to_twi(ostream& out) const;
  
  void merge(const Film& other);

  AccPixel at(int x, int y) const
  {
    return plate[index(x, y)];
  }

  AccPixel& at(int x, int y)
  {
    return plate[index(x, y)];
  }

  void clear();

  uint width, height;  

private:
  
  vector<spectrum> pixel_list() const;

  uint index(uint x, uint y) const 
  {
    assert(y < height);
    assert(x < width);
    return y * width + x;
  }
  
  vector<AccPixel> plate;
};

ostream& operator<<(ostream& o, const Film::AccPixel& rect);
ostream& operator<<(ostream& o, const Film::Rect& rect);
