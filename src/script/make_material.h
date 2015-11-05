#pragma once

class Material;
struct lua_State;

namespace script
{
  namespace material
  {
    int color(lua_State* L);
    int mirror(lua_State* L);
  }

  int color_env_light(lua_State* L);
}
