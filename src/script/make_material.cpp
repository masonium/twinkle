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

    int mirror(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 0);
      return script_material(L, make_shared<MirrorMaterial>());
    }
  }

  int color_env_light(lua_State* L)
  {
    LUA_CHECK_NUM_ARGS(L, 1);
    auto s = lua_tospectrum(L, 1);
    return script_envlight(L, make_shared<EnvironmentLight>(make_shared<SolidColor>(s)));
  }
}
