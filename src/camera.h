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
  // Return a pixel sample from anywhere on the image plane.
  virtual PixelSample sample_pixel(uint w, uint h, Sampler& sampler) const = 0;

  // Return a pixel sample that is limited to a specific pixel on the image
  // plane.
  virtual PixelSample sample_pixel(uint w, uint h, int x, int y, Sampler& sampler) const = 0;

  virtual ~Camera() { }

protected:
  // Return a random point in the image [-0.5, 0.5]^2 image plane, restricted to
  // (x, y) in the image.
  pair<scalar, scalar> to_unit_coord(uint w, uint h, int x, int y, const Sample2D& samp) const;

  PixelSample from_unit_coord(uint w, uint h, const Sample2D& samp, const Ray& r) const;
};

class PerspectiveCamera : public Camera
{
public:
  struct DepthLens
  {
    explicit DepthLens(scalar ar = 0, scalar fd = 1) : aperture_radius(ar), focal_distance(fd) { }

    scalar aperture_radius;
    scalar focal_distance;
  };

  PerspectiveCamera(Vec3 pos, Vec3 lookat_, Vec3 up, scalar fov_);
  PerspectiveCamera(Vec3 pos, Vec3 lookat_, Vec3 up, scalar fov_,
    DepthLens dl);
  
  PixelSample sample_pixel(uint w, uint h, Sampler& sampler) const override;
  PixelSample sample_pixel(uint w, uint h, int x, int y, Sampler& sampler) const override;

  Vec3 position;
  Vec3 aspect_forward;
  Vec3 up;
  Vec3 right;
  const DepthLens dl;

private:
  Ray sample_core(scalar fx, scalar fy, scalar aspect, Sampler& sampler) const;
};


class SphericalCamera : public Camera
{
public:
  SphericalCamera(Vec3 pos, Vec3 lookat_, Vec3 up);

  PixelSample sample_pixel(uint w, uint h, Sampler& sampler) const override;
  PixelSample sample_pixel(uint w, uint h, int x, int y, Sampler& sampler) const override;

private:  
  Ray sample_core(scalar fx, scalar fy) const;

  Vec3 position;
  Mat33 rot_mat;
};

