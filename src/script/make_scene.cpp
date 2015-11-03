#include <memory>
#include "make_scene.h"
#include "vec3.h"
#include "camera.h"
#include "script/make_util.h"

using std::make_shared;

namespace script
{
  int perspective_camera(lua_State* L)
  {
    LUA_CHECK_NUM_ARGS(L, 4);

    auto pos = lua_tovector(L, 1);
    auto target = lua_tovector(L, 2);
    auto up = lua_tovector(L, 3);
    auto fov = lua_tonumber(L, 4);

    auto c = make_shared<PerspectiveCamera>(pos, target, up, fov);

    return script_camera(L, c);
  }

  int spherical_camera(lua_State* L)
  {
    LUA_CHECK_NUM_ARGS(L, 3);

    auto pos = lua_tovector(L, 1);
    auto target = lua_tovector(L, 2);
    auto up = lua_tovector(L, 3);

    auto c = make_shared<SphericalCamera>(pos, target, up);

    return script_camera(L, c);
  }

}
