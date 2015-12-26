#pragma once

#include <vector>
#include <memory>
#include "hosekwilkie/ArHosekSkyModel.h"
#include "texture.h"

using std::vector;
using std::unique_ptr;


class SkyStateDeleter
{
public:
  void operator() (ArHosekSkyModelState* obj)
  {
    arhosekskymodelstate_free(obj);
  }
};


class HosekSkyTexture : public Texture2D
{
public:
  HosekSkyTexture(const Vec3& sun_dir, double turbidity, bool with_sun);

  spectrum at_coord(const Vec2& uv) const override;

private:
  vector<unique_ptr<ArHosekSkyModelState, SkyStateDeleter>> _sky_model_states;
  Vec3 _sun_direction;
  bool _with_sun;
  // vector<double> albedo;   // assume a white albedo

};
