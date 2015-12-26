#include <memory>
#include "make_scene.h"
#include "vec3.h"
#include "light.h"
#include "env_light.h"
#include "camera.h"
#include "script/script_util.h"

using std::make_shared;

namespace script
{
  namespace camera
  {
    int perspective(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 4);

      auto pos = lua_tovector(L, 1);
      auto target = lua_tovector(L, 2);
      auto up = lua_tovector(L, 3);
      auto fov = lua_tonumber(L, 4);

      auto c = make_shared<PerspectiveCamera>(pos, target, up, fov);

      return script_camera(L, c);
    }

    int spherical(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 3);

      auto pos = lua_tovector(L, 1);
      auto target = lua_tovector(L, 2);
      auto up = lua_tovector(L, 3);

      auto c = make_shared<SphericalCamera>(pos, target, up);

      return script_camera(L, c);
    }
  }


  namespace light
  {
    int point(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 2);

      auto pos = lua_tovector(L, 1);
      auto spec = lua_tospectrum(L, 2);

      auto light = make_shared<PointLight>(pos, spec);

      return script_light(L, light);
    }

    int directional(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 2);

      auto pos = lua_tovector(L, 1);
      auto spec = lua_tospectrum(L, 2);

      auto light = make_shared<DirectionalLight>(pos, spec);

      return script_light(L, light);
    }

    int environment(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 1);

      auto tex = lua_totexture(L, 1);
      auto tex2d = reinterpret_cast<const Texture2D*>(tex);
      assert(tex2d);
      auto light = make_shared<EnvironmentLight>(tex2d);

      return script_light(L, light);
    }

  }
}
