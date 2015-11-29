#pragma once

#include "spectrum.h"
#include <vector>

using std::vector;

struct Image
{
  int width;
  int height;
  vector<spectrum> data;
};

bool load_image(const std::string&, Image& img);

bool save_image(const std::string&, const Image&);
bool save_image(const std::string&, const Image&, std::string& format);
