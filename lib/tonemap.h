#pragma once

#include "spectrum.h"
#include <vector>
#include <numeric>

using std::vector;
using std::weak_ptr;

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

class RSSFToneMapper : public ToneMapper
{
public:
  RSSFToneMapper(scalar mid = 0.18) : middle_luminance(mid) {}
  
  void tonemap(const vector<spectrum>& input, vector<spectrum>& output,
               uint w, uint h) const override;

private:
  scalar middle_luminance;
};

class CompositeToneMapper : public ToneMapper
{
public:
  CompositeToneMapper(std::initializer_list<weak_ptr<ToneMapper>> lst);

  void tonemap(const vector<spectrum>& input, vector<spectrum>& output,
               uint w, uint h) const override;

private:
  vector<weak_ptr<ToneMapper>> mappers;
};
