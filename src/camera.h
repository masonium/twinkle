#pragma once

#include <vector>
#include <iostream>
#include "math_util.h"
#include "spectrum.h"
#include "film.h"
#include "ray.h"
#include "sampler.h"
#include "mat33.h"

using std::vector;
using std::ostream;

class Camera
{
public:
  virtual PixelSample sample_pixel(const Film& f, int x, int y, Sampler& sampler) const = 0;
  virtual ~Camera() { }

protected:
  pair<scalar, scalar> to_unit_coord(const Film& f, int x, int y, Sample2D samp) const;
};

class PerspectiveCamera : public Camera
{
public:
  PerspectiveCamera(Vec3 pos, Vec3 lookat_, Vec3 up,
         scalar fov_, scalar aspect_,
         scalar aperture_radius_ = 0.0, scalar f = 1.0);
  
  PixelSample sample_pixel(const Film& f, int x, int y, Sampler& sampler) const override;

private:
  Vec3 position;
  Vec3 aspect_forward;
  Vec3 up;
  Vec3 right;
  scalar aspect;
  const scalar aperture, focal_length;
};


class SphericalCamera : public Camera
{
public:
  SphericalCamera(Vec3 pos, Vec3 lookat_, Vec3 up);
                  
  PixelSample sample_pixel(const Film& f, int x, int y, Sampler& sampler) const override;

private:  
  Vec3 position;
  Mat33 rot_mat;
};

