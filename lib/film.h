#pragma once

#include "spectrum.h"
#include <iostream>
#include <cassert>
#include <vector>

using std::vector;
using std::ostream;

class ImageSampleFilter
{
public:
  virtual spectrum combine_samples(const vector<spectrum>& samples) const = 0;
};


class BoxFilter : public ImageSampleFilter
{
public:
  spectrum combine_samples(const vector<spectrum>& samples) const override;
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

class Film
{
public:  
  Film(uint w_, uint h_);
  
  void add_sample(int x, int y, const spectrum& s);
  
  void render_to_console(ostream& out);
  void render_to_ppm(ostream& out, ImageSampleFilter* filter, ToneMapper* mapper);
  
  const uint width, height;
  
private:
  uint index(uint x, uint y) const 
  {
    assert(0 <= y && y < height);
    assert(0 <= x && x < width);
    return y * width + x;
  }
  
  vector<vector<spectrum>> plate;
};
