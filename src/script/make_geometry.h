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
    int plane(lua_State*);
    int implicit(lua_State*);
  }
}
