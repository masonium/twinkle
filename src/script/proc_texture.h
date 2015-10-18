#pragma once

#include <string>
#include "texture.h"

class ScriptTexture : public Texture2D
{
public:
  ScriptTexture(const std::string& fn_name);

  spectrum at_coord(const Vec2& uv) const override;

private:
  std::string _fn_name;
};

