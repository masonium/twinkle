#pragma once

#include <lua.hpp>

namespace script
{
  namespace camera
  {
    int perspective(lua_State* L);
    int spherical(lua_State* L);
  }

  namespace light
  {
    int point(lua_State* L);
    int directional(lua_State* L);
  }

  namespace env_light
  {
    int color(lua_State* L);
//    int texture(lua_State* L);
  }
}
