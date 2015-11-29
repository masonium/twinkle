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

      return script_bbox(L, ::bounds::AABB(lower, upper));
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

    int quad(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 2);
      auto v1 = lua_tovector(L, 1);
      auto v2 = lua_tovector(L, 2);

      return script_geometry(L, make_quad(v1, v2));
    }

    int plane(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 2);

      auto v = lua_tovector(L, 1);
      scalar d = lua_tonumber(L, 2);

      return script_geometry(L, make_shared<Plane>(v, d));
    }

    int mesh(lua_State* L)
    {
      LUA_CHECK_RANGE_ARGS(L, 1, 2);

      const char* s = lua_tostring(L, 1);

      if (lua_gettop(L) == 2)
      {
        auto box = lua_tobbox(L, 2);
        return script_geometry(L, load_mesh(s, box));
      }
      else
      {
        return script_geometry(L, load_mesh(s));
      }
    }

    int implicit(lua_State* L)
    {
      using namespace std::string_literals;

      LUA_CHECK_RANGE_ARGS(L, 3, 4);

      int argnum = 0;
      auto name = std::string{lua_tostring(L, ++argnum)};
      auto grad_name = ""s;
      if (lua_gettop(L) == 4)
      {
        grad_name = lua_tostring(L, ++argnum);
      }
      scalar lc = lua_tonumber(L, ++argnum);
      auto box = lua_tobbox(L, ++argnum);

      if (grad_name.empty())
        return script_geometry(L, make_shared<Implicit>(name, lc, box));
      else
        return script_geometry(L, make_shared<ImplicitWithGrad>(name, grad_name, lc, box));
    }
  }
}


