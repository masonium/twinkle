#pragma once

struct lua_State;

namespace script
{
  /*
    Create a sphere.
   */
  int sphere(lua_State*);

  /*
    Create a plane.
   */
  int plane(lua_State*);
}
