#pragma once

#include "texture.h"

struct sky_distribution_coef
{
  sky_distribution_coef() {}
  sky_distribution_coef(scalar a_, scalar b_, scalar c_, scalar d_, scalar e_);
  sky_distribution_coef(const scalar coef_[5]);

  union
  {
    struct
    {
      scalar a, b, c, d, e;
    };
    scalar v[5];
  };
};

const scalar TURBIDITY_PURE = 1;
const scalar TURBIDITY_VERY_CLEAR = 2;
const scalar TURBIDITY_LIGHT_HAZE = 20;
const scalar TURBIDITY_THIN_FOG = 64;

class ShirleySkyTexture : public Texture2D
{
public:
  ShirleySkyTexture(const Vec3& sun_dir, scalar turbidity);

  spectrum at_coord(const Vec2& uv) const override;

private:

  static spectrum_xyY sun_spectrum_xyY(scalar turbidity, scalar angle_from_top);

  Vec3 sun_dir;
  spectrum_xyY sun_spec_xyY;
  spectrum_xyY base_dist;

  sky_distribution_coef x_coef, y_coef, Y_coef;

  spectrum_xyY at_coord_xyY(scalar cos_phi, scalar cos_sun) const;
};

scalar luminance_gradient(scalar cos_phi, const sky_distribution_coef& sdc);
scalar scattering_indicatrix(scalar cos_angle_from_sun, const sky_distribution_coef& sdc);

scalar sky_luminance_distribution(scalar cos_phi, scalar cos_angle_from_sun,
                                  const sky_distribution_coef& sdc);
