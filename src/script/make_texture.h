#pragma once

#include <lua.hpp>

namespace script
{
  namespace texture
  {
    int color(lua_State* L);
    int checker(lua_State* L);
    int grid(lua_State* L);
    int gradient(lua_State* L);
    int image(lua_State* L);
    int normal(lua_State* L);
    int hoseksky(lua_State* L);
  }
}
