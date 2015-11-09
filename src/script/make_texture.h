#pragma once

#include <lua.hpp>

namespace script
{
  namespace texture
  {
    int color(lua_State* L);
    int checker(lua_State* L);
    int grid(lua_State* L);
  }
}
