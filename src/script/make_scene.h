#pragma once

#include <lua.hpp>

namespace script
{
  namespace camera
  {
    int perspective(lua_State* L);
    int spherical(lua_State* L);
  }
}
