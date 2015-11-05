#pragma once

struct lua_State;

namespace script
{
  namespace bounds
  {
    int box(lua_State*);
  }

  namespace geometry
  {
    int sphere(lua_State*);
    int plane(lua_State*);
  }
}
