#include "skytexture.h"

using std::copy;


scalar luminance_gradient(scalar cos_phi, const sky_distribution_coef& sdc)
{
  return 1 + sdc.a * exp(sdc.b / cos_phi);
}

scalar scattering_indicatrix(scalar cos_afs, const sky_distribution_coef& sdc)
{

  scalar afs = acos(cos_afs);
  return 1 + sdc.c * exp(sdc.d * afs) + sdc.e * cos_afs * cos_afs;
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

const scalar X_LDF_LINEAR[5] = {-0.0193, -0.0665, -0.0004, -0.0641, -0.0033};
const scalar X_LDF_CONST[5] = {-0.2592, 0.0008, 0.2125, -0.8989, 0.0452};

const scalar Y_LDF_LINEAR[5] = {-0.0167, -0.0950, -0.0079, -0.0441, -0.0109};
const scalar Y_LDF_CONST[5] = {-0.2608, 0.0092, 0.2102, -1.6537, 0.0529};

const scalar YY_LDF_LINEAR[5] = {0.1787, -0.3554, -0.0227, 0.1206, -0.0670};
const scalar YY_LDF_CONST[5] = {-1.4630, 0.4275, 5.3251, -2.5771, 0.3703};

const scalar X_ZENITH[3][4] = {{0.00166, -0.00375, 0.00209, 0},
                               {-0.02903, 0.06377, -0.03202, 0.00394},
                               {0.11693, -0.21196, 0.06052, 0.25886}};

const scalar Y_ZENITH[3][4] = {{0.00275, -0.00610, 0.00317, 0},
                               {-0.04214, 0.08970, -0.04153, 0.00516},
                               {0.15346, -0.26756, 0.06670, 0.26688}};


ShirleySkyTexture::ShirleySkyTexture(const Vec3& sun_dir_, scalar turbidity)
  : sun_dir(sun_dir_.normal())
{
  scalar coef[3][5];
  for (int i = 0; i < 5; ++i)
  {
    coef[0][i] = X_LDF_LINEAR[i] * turbidity + X_LDF_CONST[i];
    coef[1][i] = Y_LDF_LINEAR[i] * turbidity + Y_LDF_CONST[i];
    coef[2][i] = YY_LDF_LINEAR[i] * turbidity + YY_LDF_CONST[i];
  }

  x_sky_coef = sky_distribution_coef(coef[0]);
  y_sky_coef = sky_distribution_coef(coef[1]);
  Y_sky_coef = sky_distribution_coef(coef[2]);

  auto sun_spec_xyY = sun_spectrum_xyY(turbidity, sun_dir.to_euler().second);
  auto base_ratio = sky_spectrum_xyY(cos(0), cos(0));

  point_to_sun_ratio.x = sun_spec_xyY.x / base_ratio.x;
  point_to_sun_ratio.y = sun_spec_xyY.y / base_ratio.y;
  point_to_sun_ratio.Y = sun_spec_xyY.Y / base_ratio.Y;
}

spectrum_xyY ShirleySkyTexture::sun_spectrum_xyY(scalar turbidity, scalar angle)
{
  scalar turb_vec[3] = {turbidity * turbidity, turbidity, 1};
  scalar angle_vec[4] = {angle * angle * angle, angle * angle, angle, 1};
  scalar x = 0, y = 0;
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      x += turb_vec[i] * X_ZENITH[i][j] * angle_vec[j];
      y += turb_vec[i] * Y_ZENITH[i][j] * angle_vec[j];
    }
  }
  scalar lum = zenith_luminance(turbidity, angle);

  return spectrum_xyY(x, y, lum);
}

spectrum_xyY ShirleySkyTexture::sky_spectrum_xyY(scalar cos_phi, scalar cos_sun) const
{
  return spectrum_xyY(sky_luminance_distribution(cos_phi, cos_sun, x_sky_coef),
                      sky_luminance_distribution(cos_phi, cos_sun, y_sky_coef),
                      sky_luminance_distribution(cos_phi, cos_sun, Y_sky_coef));
}

spectrum ShirleySkyTexture::at_coord(const Vec2& uv) const
{
  auto dir = Vec3::from_euler(uv.u * 2 * M_PI, uv.v * PI);

  auto cos_phi = dir.z;
  if (cos_phi < 0)
    return spectrum::zero;
  auto cos_sun = dir.dot(sun_dir);

  auto ss = sky_spectrum_xyY(cos_phi, cos_sun);

  auto xyY = spectrum_xyY{ss.x * point_to_sun_ratio.x,
                          ss.y * point_to_sun_ratio.y,
                          ss.Y * point_to_sun_ratio.Y};

  return static_cast<spectrum_XYZ>(xyY);
}
