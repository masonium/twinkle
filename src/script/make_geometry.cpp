#include <lua.hpp>
#include "geometries.h"
#include "bounds.h"
#include "script/script_util.h"
#include "script/make_geometry.h"

using std::make_shared;

namespace script
{
  namespace bounds
  {
    int bbox(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 2);

      auto lower = lua_tovector(L, 1);
      auto upper = lua_tovector(L, 2);

      return script_bbox(L, make_shared<::bounds::AABB>(lower, upper));
    }
  }

  namespace geometry
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

    int implicit(lua_State* L)
    {
      LUA_CHECK_RANGE_ARGS(L, 2, 3);

      auto name = std::string{lua_tostring(L, 1)};
      auto box = lua_tobbox(L, 2);
      scalar lc = 1.0;

      if (lua_gettop(L) == 3)
        lc = lua_tonumber(L, 3);

      return 0;
    }
  }
}


