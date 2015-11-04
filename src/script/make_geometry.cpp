#include <lua.hpp>
#include "geometries.h"
#include "script/script_util.h"
#include "script/make_geometry.h"

using std::make_shared;

namespace script
{
  int sphere(lua_State* L)
  {
    LUA_CHECK_NUM_ARGS(L, 2);

    auto v = lua_tovector(L, 1);
    scalar r = lua_tonumber(L, 2);
    
    return script_geometry(L, make_shared<Sphere>(v, r));
  }

  int plane(lua_State* L)
  {
    LUA_CHECK_NUM_ARGS(L, 2);

    auto v = lua_tovector(L, 1);
    scalar d = lua_tonumber(L, 2);

    return script_geometry(L, make_shared<Plane>(v, d));
  }
}


