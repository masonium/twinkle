#include "camera.h"

PerspectiveCamera::PerspectiveCamera(Vec3 pos, Vec3 lookat_, Vec3 up_,
                                     scalar fov_, scalar aspect_, scalar aperture_radius_,
                                     scalar f) :
  position(pos), aspect(aspect_), aperture(aperture_radius_), focal_length(f)
{
  const Vec3 forward = lookat_ - pos;
  right = forward.cross(up_).normal();
  up = right.cross(forward).normal();
  aspect_forward = forward.normal() * 0.5 / tan(fov_ / 2); 
}

PixelSample PerspectiveCamera::sample_pixel(Film* f, int x, int y, const Sample5D& samp)
{
  const scalar dw = 1.0 / f->width, dh = 1.0 / f->height;
  const scalar fx = (x + samp[0]) * dw - 0.5, fy = (y + samp[1]) * dh - 0.5;

  Ray r{ position, up * fy + right * fx * aspect + aspect_forward };
  
  if (aperture > 0)
  {
    r.normalize();
    auto focal_pos = r.evaluate(focal_length);

    const scalar lens_angle = 2 * PI * samp[3];
    const scalar lens_radius = aperture * sqrt( samp[4] );

    auto new_origin = position + right * lens_radius * cos(lens_angle) +
      up * lens_radius * sin(lens_angle);
    r = Ray{ new_origin, focal_pos - new_origin };
  }

  return PixelSample(x, y, fx, fy, r);
}
