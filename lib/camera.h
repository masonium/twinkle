#pragma once

#include <vector>
#include <iostream>
#include "math_util.h"

using std::vector;
using std::ostream;
typedef scalar spectrum;

class Film
{
public:  
  Film(uint w_, uint h_) : width(w_), height(h_), plate(w_ * h_)
  {
    
  }

  spectrum& operator()(uint x, uint y)
  {
    return at(x, y);
  }
  spectrum operator()(uint x, uint y) const
  {
    return at(x, y);
  }
  
  spectrum& at(uint x, uint y)
  {
    return plate[index(x, y)];
  }
  
  spectrum at(uint x, uint y) const
  {
    return plate[index(x, y)];
  }

  void render(ostream& out);
  
  const uint width, height;
  
private:
  uint index(uint x, uint y) const 
  {
    return y * width + x;
  }
  
  vector<spectrum> plate;
};

class Camera
{
public:
  Camera(Vec3 pos, Vec3 lookat_, Vec3 up,
         scalar fov_, scalar aspect_);
  
  Ray sample_pixel_ray(Film* f, int x, int y);

  Vec3 position;
  Vec3 aspect_forward;
  Vec3 up;
  Vec3 right;
  scalar aspect;
};
