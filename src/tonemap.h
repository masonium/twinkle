#pragma once

#include "spectrum.h"
#include "util/array.h"
#include "util/images.h"
#include <vector>
#include <numeric>

using std::vector;
using std::shared_ptr;

class ToneMapper
{
public:
  virtual sp_image tonemap(const sp_image& input) const = 0;

  virtual ~ToneMapper() { }
};

class CutoffToneMapper : public ToneMapper
{
public:
  sp_image tonemap(const sp_image& input) const override;
};

class LinearToneMapper : public ToneMapper
{
public:
  sp_image tonemap(const sp_image& input) const override;
};

class CompositeToneMapper : public ToneMapper
{
public:
  CompositeToneMapper(std::initializer_list<shared_ptr<ToneMapper>> lst);

  sp_image tonemap(const sp_image& input) const override;

private:
  vector<shared_ptr<ToneMapper>> mappers;
};
