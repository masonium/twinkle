#pragma once

struct lua_State;

namespace script
{
  namespace bounds
  {
    int bbox(lua_State*);
  }

  namespace geometry
  {
    int sphere(lua_State*);
    int quad(lua_State*);
    int box(lua_State*);
    int plane(lua_State*);
    int implicit(lua_State*);
    int mesh(lua_State*);
    int heightfield(lua_State*);
    int parametric(lua_State*);
  }
}
