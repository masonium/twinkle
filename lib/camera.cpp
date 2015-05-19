#include "camera.h"

void Film::render(ostream& out)
{
  for (int y = height - 1; y >= 0; --y)
  {
    for (uint x = 0; x < width; ++x)
    {
      spectrum s = at(x, y);
      if (s > 0.5)
        out << '*';
      else if (s > 0.25)
        out << '.';
      else
        out << ' ';
    }
    out << '\n';
  }
}

////////////////////////////////////////////////////////////////////////////////
Camera::Camera(Vec3 pos, Vec3 lookat_, Vec3 up_,
               scalar fov_, scalar aspect_) :
  position(pos), aspect(aspect_)
{
  const Vec3 forward = lookat_ - pos;
  right = forward.cross(up_).normal();
  up = right.cross(forward).normal();
  aspect_forward = forward.normal() * 0.5 / tan(fov_ / 2); 
}

Ray Camera::sample_pixel_ray(Film* f, int x, int y)
{
  const scalar dw = 1.0 / f->width, dh = 1.0 / f->height;
  const scalar fx = (x + 0.5) * dw - 0.5, fy = (y + 0.5) * dh - 0.5;
  return Ray( position, up * fy + right * fx * aspect + aspect_forward );
}
