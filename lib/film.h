#pragma once

#include "math_util.h"
#include "spectrum.h"
#include <iostream>
#include <cassert>
#include <vector>

using std::vector;
using std::ostream;

class Film;

struct PixelSample
{
  PixelSample(int x_, int y_, scalar px_, scalar py_, Ray ray_) :
    x(x_), y(y_), px(px_), py(py_), ray(ray_)
  {
  }

  int x, y;
  scalar px, py;
  Ray ray;
};

class ImageSampleFilter
{
public:
  virtual void add_sample(Film* film, const PixelSample& p, const spectrum& s) const = 0;
};


class BoxFilter : public ImageSampleFilter
{
public:
  void add_sample(Film* film, const PixelSample& p, const spectrum& s) const override;
};

class ToneMapper
{
public:
  virtual void tonemap(const vector<spectrum>& input, vector<spectrum>& output,
                       uint w, uint h) const = 0;
};

class CutoffToneMapper : public ToneMapper
{
public:
  void tonemap(const vector<spectrum>& input, vector<spectrum>& output,
               uint w, uint h) const override;
};

class LinearToneMapper : public ToneMapper
{
public:
  void tonemap(const vector<spectrum>& input, vector<spectrum>& output,
               uint w, uint h) const override;
};

struct FilmPixel
{
  scalar weight;
  spectrum total;
};

class Film
{
public:  
  Film(uint w_, uint h_, ImageSampleFilter* f);
  
  void add_sample(const PixelSample& ps, const spectrum& s);
  
  void render_to_console(ostream& out);
  void render_to_ppm(ostream& out, ToneMapper* mapper);
  
  FilmPixel at(int x, int y) const
  {
    return plate[index(x, y)];
  }

  FilmPixel& at(int x, int y)
  {
    return plate[index(x, y)];
  }

  const uint width, height;  

private:
  vector<spectrum> pixel_list() const;

  uint index(uint x, uint y) const 
  {
    assert(0 <= y && y < height);
    assert(0 <= x && x < width);
    return y * width + x;
  }
  
  vector<FilmPixel> plate;
  ImageSampleFilter* filter;
};
