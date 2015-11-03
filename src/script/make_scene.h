#pragma once

#include <lua.hpp>

namespace script
{
  int perspective_camera(lua_State* L);
  int spherical_camera(lua_State* L);
}
