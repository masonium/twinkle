#pragma once

#include <memory>
#include "spectrum.h"
#include "vec3.h"
#include "texture.h"

using std::shared_ptr;

class EnvironmentalLight
{
public:
  EnvironmentalLight(shared_ptr<Texture2D> tex) : texture(tex) { }

  spectrum emission(const Vec3& dir) const;

  shared_ptr<Texture2D> texture;
};
