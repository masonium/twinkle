#pragma once

#include "util/array.h"
#include "spectrum.h"
#include <vector>

using std::vector;

class sp_image : public Array2D<spectrum>
{
public:
  sp_image() {}

  using Base = Array2D<spectrum>;
  sp_image(size_t w, size_t h) : Base(w, h)
  {
  }

  template <typename container_type>
  sp_image(size_t w, size_t h, container_type&& data) :
    Base(w, h, data)
  {
  }

  template <typename container_type>
  sp_image(container_type&& b) : Base(b.width(), b.height(), b)
  {
  }
};

sp_image load_image(const std::string&);

bool save_image(const std::string&, const sp_image&);

void ascii_image(ostream& out, const sp_image&);
