#include <lua.hpp>
#include "geometries.h"
#include "bounds.h"
#include "script/script_util.h"
#include "script/make_geometry.h"
#include "script/proc_implicit.h"

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
      LUA_CHECK_NUM_ARGS(L, 3);

      auto name = std::string{lua_tostring(L, 1)};
      scalar lc = lua_tonumber(L, 2);
      auto box = lua_tobbox(L, 3);
      cerr << box->to_string() << endl;

      return script_geometry(L, make_shared<Implicit>(name, lc, *box));
    }
  }
}


