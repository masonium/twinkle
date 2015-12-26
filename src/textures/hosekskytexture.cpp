#include <cassert>
#include "textures/hosekskytexture.h"

HosekSkyTexture::HosekSkyTexture(const Vec3& sun_dir, double turbidity, bool with_sun) :
  _sun_direction(sun_dir.normal()), _with_sun(with_sun)
{
  assert(!with_sun);

  // We can use a tristim-model with the sun. Otherwise, we have to use the
  // standard model.
  if (with_sun)
  {

  }
  else
  {
    auto ptr = arhosek_rgb_skymodelstate_alloc_init(turbidity, 1.0, acos(_sun_direction.z));
    _sky_model_states.emplace_back(ptr);
  }
}

spectrum HosekSkyTexture::at_coord(const Vec2& uv) const
{
  spectrum s{0.0};
  scalar elev_angle = uv.v * M_PI;
  if (elev_angle > M_PI / 2.0)
    return s;

  scalar gamma = acos(from_euler_uv(uv).dot(_sun_direction));

  for (int i = 0; i < 3; ++i)
  {
    s[i] = arhosek_tristim_skymodel_radiance(_sky_model_states[0].get(),
                                             elev_angle, gamma, i);
  }
  return s;
}
