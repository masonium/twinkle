#pragma once

class Material;
struct lua_State;

namespace script
{
  int color_material(lua_State* L);
  int color_env_light(lua_State* L);
}
