#include "camera.h"

using std::tie;
using std::make_pair;

pair<scalar, scalar> Camera::to_unit_coord(uint w, uint h, int x, int y, Sample2D samp) const
{
  const scalar dw = 1.0 / w, dh = 1.0 / h;
  const scalar fx = (x + samp[0]) * dw - 0.5, fy = (y + samp[1]) * dh - 0.5;
  return make_pair(fx, fy);
}


PerspectiveCamera::PerspectiveCamera(Vec3 pos, Vec3 lookat_, Vec3 up_,
                                     scalar fov_) :
  PerspectiveCamera(pos, lookat_, up_, fov_, DepthLens())
{
}

PerspectiveCamera::PerspectiveCamera(Vec3 pos, Vec3 lookat_, Vec3 up_,
                                     scalar fov_, DepthLens depth_lens)
{
  const Vec3 forward = lookat_ - pos;
  right = forward.cross(up_).normal();
  up = right.cross(forward).normal();
  aspect_forward = forward.normal() * 0.5 / tan(fov_ / 2); 
}

PixelSample PerspectiveCamera::sample_pixel(uint w, uint h, int x, int y, Sampler& sampler) const
{
  auto ps = sampler.sample_2d();
  scalar fx, fy;
  tie(fx, fy) = to_unit_coord(w, h, x, y, ps);

  scalar aspect = scalar(w)/scalar(h);
  Ray r{ position, up * fy + right * fx * aspect + aspect_forward };
  
  if (dl.aperture_radius > 0)
  {
    auto lens_samp = sampler.sample_2d();
    r.normalize();
    auto focal_pos = r.evaluate(dl.focal_distance);

    const scalar lens_angle = 2 * PI * lens_samp[0];
    const scalar lens_radius = dl.aperture_radius * sqrt(lens_samp[1]);

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

PixelSample SphericalCamera::sample_pixel(uint w, uint h, int x, int y, Sampler& sampler) const
{
  auto ps = sampler.sample_2d();
  scalar fx, fy;
  tie(fx, fy) = to_unit_coord(w, h, x, y, ps);

  const scalar phi = (0.5 - fy) * PI, theta = (fx + 0.5) * (2 * PI);

  auto dir = Vec3::from_euler(theta, phi);

  return PixelSample(x, y, fx, fy, Ray(position, rot_mat * dir));
}
