#include "camera.h"

Camera::Camera(Vec3 pos, Vec3 lookat_, Vec3 up_,
               scalar fov_, scalar aspect_) :
  position(pos), aspect(aspect_)
{
  const Vec3 forward = lookat_ - pos;
  right = forward.cross(up_).normal();
  up = right.cross(forward).normal();
  aspect_forward = forward.normal() * 0.5 / tan(fov_ / 2); 
}

PixelSample Camera::sample_pixel(Film* f, int x, int y, scalar r1, scalar r2)
{
  const scalar dw = 1.0 / f->width, dh = 1.0 / f->height;
  const scalar fx = (x + r1) * dw - 0.5, fy = (y + r2) * dh - 0.5;
  Ray r{ position, up * fy + right * fx * aspect + aspect_forward };

  return PixelSample(x, y, fx, fy, r);
}
