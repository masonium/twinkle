#include "camera.h"

using std::tie;
using std::make_pair;

pair<scalar, scalar> Camera::to_unit_coord(const Film& f, int x, int y, Sample2D samp) const
{
  const scalar dw = 1.0 / f.width, dh = 1.0 / f.height;
  const scalar fx = (x + samp[0]) * dw - 0.5, fy = (y + samp[1]) * dh - 0.5;
  return make_pair(fx, fy);
}


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

PixelSample PerspectiveCamera::sample_pixel(const Film& f, int x, int y, Sampler& sampler) const
{
  auto ps = sampler.sample_2d();
  scalar fx, fy;
  tie(fx, fy) = to_unit_coord(f, x, y, ps);

  Ray r{ position, up * fy + right * fx * aspect + aspect_forward };
  
  if (aperture > 0)
  {
    auto lens_samp = sampler.sample_2d();
    r.normalize();
    auto focal_pos = r.evaluate(focal_length);

    const scalar lens_angle = 2 * PI * lens_samp[0];
    const scalar lens_radius = aperture * sqrt(lens_samp[1]);

    auto new_origin = position + right * lens_radius * cos(lens_angle) +
      up * lens_radius * sin(lens_angle);
    r = Ray{ new_origin, focal_pos - new_origin };
  }

  return PixelSample(x, y, fx, fy, r);
}

SphericalCamera::SphericalCamera(Vec3 pos, Vec3 lookat_, Vec3 up_) :
  position(pos)
{
  const auto forward = (lookat_ - pos).normal();
  const auto right = forward.cross(up_).normal();
  const auto up = right.cross(forward).normal();

  rot_mat = Mat33(right, forward, up).transpose();
}

PixelSample SphericalCamera::sample_pixel(const Film& f, int x, int y, Sampler& sampler) const
{
  auto ps = sampler.sample_2d();
  scalar fx, fy;
  tie(fx, fy) = to_unit_coord(f, x, y, ps);

  const scalar phi = (0.5 - fy) * PI, theta = (fx + 0.5) * (2 * PI);

  auto dir = Vec3::from_euler(theta, phi);

  return PixelSample(x, y, fx, fy, Ray(position, rot_mat * dir));
}
