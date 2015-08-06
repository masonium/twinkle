#include "skytexture.h"

using std::copy;


scalar luminance_gradient(scalar cos_phi, const sky_distribution_coef& sdc)
{
  return 1 + sdc.a * exp(sdc.b / cos_phi);
}

scalar scattering_indicatrix(scalar cos_afs, const sky_distribution_coef& sdc)
{

  scalar afs = acos(cos_afs);
  return 1 + sdc.c * exp(sdc.d * afs)  + sdc.e * cos_afs * cos_afs;
}

scalar sky_luminance_distribution(scalar cos_phi, scalar cos_angle_from_sun,
                                  const sky_distribution_coef& sdc)
{
  return luminance_gradient(cos_phi, sdc) * scattering_indicatrix(cos_angle_from_sun, sdc);
}

////////////////////////////////////////////////////////////////////////////////


sky_distribution_coef::sky_distribution_coef(scalar a_, scalar b_, scalar c_, scalar d_, scalar e_)
  : a(a_), b(b_), c(c_), d(d_), e(e_)
{

}

sky_distribution_coef::sky_distribution_coef(const scalar coef_[5])
{
  copy(coef_, coef_ + 5, v);
}

scalar zenith_luminance(scalar turbidity, scalar angle_from_top)
{
  scalar chi = (4.0 / 9.0 - turbidity / 120) * (PI - 2 * angle_from_top);

  return (4.0453 * turbidity - 4.9710) * tan(chi) - 0.2155 * turbidity + 0.24192;
}

scalar x_ldf_t[5] = {-0.0193, -0.0665, -0.0004, -0.0641, -0.0033};
scalar x_ldf_const[5] = {-0.2592, 0.0008, 0.2125, -0.8989, 0.0452};

scalar y_ldf_t[5] = {-0.0167, -0.0950, -0.0079, -0.0441, -0.0109};
scalar y_ldf_const[5] = {-0.2608, 0.0092, 0.2102, -1.6537, 0.0529};

scalar Y_ldf_t[5] = {0.1787, -0.3554, -0.0227, 0.1206, -0.0670};
scalar Y_ldf_const[5] = {-1.4630, 0.4275, 5.3251, -2.5771, 0.3703};

scalar x_zenith[3][4] = {{0.00166, -0.00375, 0.00209, 0},
{-0.02903, 0.06377, -0.03202, 0.00394},
{0.11693, -0.21196, 0.06052, 0.25886}};

scalar y_zenith[3][4] = {{0.00275, -0.00610, 0.00317, 0},
{-0.04214, 0.08970, -0.04153, 0.00516},
{0.15346, -0.26756, 0.06670, 0.26688}};


ShirleySkyTexture::ShirleySkyTexture(const Vec3& sun_dir_, scalar turbidity)
  : sun_dir(sun_dir_.normal()), sun_spec_xyY(sun_spectrum_xyY(turbidity, sun_dir.to_euler().second))
{
  scalar coef[3][5];
  for (int i = 0; i < 5; ++i)
  {
    coef[0][i] = x_ldf_t[i] * turbidity + x_ldf_const[i];
    coef[1][i] = y_ldf_t[i] * turbidity + y_ldf_const[i];
    coef[2][i] = Y_ldf_t[i] * turbidity + Y_ldf_const[i];
  }

  x_coef = sky_distribution_coef(coef[0]);
  y_coef = sky_distribution_coef(coef[1]);
  Y_coef = sky_distribution_coef(coef[2]);

  base_dist.x = sky_luminance_distribution(1, 1, x_coef);
  base_dist.y = sky_luminance_distribution(1, 1, y_coef);
  base_dist.Y = sky_luminance_distribution(1, 1, Y_coef);
}

spectrum_xyY ShirleySkyTexture::sun_spectrum_xyY(scalar turbidity, scalar angle)
{
  scalar turb_vec[3] = {turbidity * turbidity, turbidity, 1};
  scalar angle_vec[4] = {angle*angle*angle, angle*angle, angle, 1};
  scalar x = 0, y = 0;
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      x += turb_vec[i] * x_zenith[i][j] * angle_vec[j];
      y += turb_vec[i] * y_zenith[i][j] * angle_vec[j];
    }
  }
  scalar lum = zenith_luminance(turbidity, angle);

  return spectrum_xyY(x, y, lum);
}

spectrum_xyY ShirleySkyTexture::at_coord_xyY(scalar cos_phi, scalar cos_sun) const
{
  scalar c[3] = { sky_luminance_distribution(cos_phi, cos_sun, x_coef),
      sky_luminance_distribution(cos_phi, cos_sun, y_coef),
      sky_luminance_distribution(cos_phi, cos_sun, Y_coef)};

  return spectrum_xyY{ c[0] / base_dist.x * sun_spec_xyY.x,
      c[1] / base_dist.y * sun_spec_xyY.y,
      c[2] / base_dist.Y * sun_spec_xyY.Y};
}


spectrum ShirleySkyTexture::at_coord(const Vec2& uv) const
{
  auto dir = Vec3::from_euler(uv.u * 2 * M_PI, uv.v * PI);

  auto cos_phi = dir.z;
  if (cos_phi < 0)
    return spectrum::zero;
  auto cos_sun_angle = dir.dot(sun_dir);

  return spectrum(at_coord_xyY(cos_phi, cos_sun_angle));
}
