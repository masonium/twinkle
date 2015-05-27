#pragma once

#include <vector>
#include <iostream>
#include "math_util.h"
#include "spectrum.h"
#include "film.h"
#include "ray.h"

using std::vector;
using std::ostream;

class Camera
{
public:
  Camera(Vec3 pos, Vec3 lookat_, Vec3 up,
         scalar fov_, scalar aspect_);
  
  Ray sample_pixel_ray(Film* f, int x, int y, scalar r1, scalar r2);

  Vec3 position;
  Vec3 aspect_forward;
  Vec3 up;
  Vec3 right;
  scalar aspect;
};
