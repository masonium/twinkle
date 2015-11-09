#include <lua.hpp>
#include <memory>
#include "material.h"
#include "env_light.h"
#include "script/make_material.h"
#include "script/script_util.h"

using std::shared_ptr;
using std::make_shared;

namespace script
{
  namespace material
  {
    int color(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 1);
      auto s = lua_tospectrum(L, 1);
      return script_material(L, make_shared<RoughColorMaterial>(0.0, s));
    }

    int diffuse(lua_State* L)
    {
      LUA_CHECK_RANGE_ARGS(L, 1, 2);
      auto tex = lua_totexture(L, 1);
      scalar roughness = 0.0;
      if (lua_gettop(L) == 2)
        roughness = lua_tonumber(L, 2);

      return script_material(L, make_shared<RoughMaterial>(roughness, tex));
    }

    int mirror(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 0);
      return script_material(L, make_shared<MirrorMaterial>());
    }
  }

}
