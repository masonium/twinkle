#pragma once

class Material;
struct lua_State;

namespace script
{
  namespace material
  {
    int color(lua_State* L);
    int glossy_paint(lua_State* L);
    int dielectric(lua_State* L);
    int diffuse(lua_State* L);
    int mirror(lua_State* L);
    int emissive(lua_State* L);
  }
}
