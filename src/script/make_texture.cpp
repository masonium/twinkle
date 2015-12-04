#include "script/make_texture.h"
#include "script/script_util.h"
#include "textures.h"

using std::make_shared;

namespace script
{
  namespace texture
  {
    int color(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 1);

      auto spec = lua_tospectrum(L, 1);

      return script_texture(L, make_shared<SolidColor>(spec));
    }

    int normal(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 0);

      return script_texture(L, make_shared<NormalTexture>());
    }

    int gradient(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 0);

      return script_texture(L, make_shared<Gradient2D>());
    }

    int checker(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 3);

      auto s1 = lua_tospectrum(L, 1);
      auto s2 = lua_tospectrum(L, 2);
      auto gs = luaL_checknumber(L, 3);

      return script_texture(L, make_shared<Checkerboard2D>(s1, s2, gs));
    }

    int grid(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 4);

      auto s1 = lua_tospectrum(L, 1);
      auto s2 = lua_tospectrum(L, 2);
      auto gs = lua_tonumber(L, 3);
      auto border_pct = lua_tonumber(L, 4);

      return script_texture(L, make_shared<GridTexture2D>(s1, s2, gs, border_pct));
    }

    int image(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 1);

      auto filename = lua_tostring(L, 1);

      return script_texture(L, make_shared<ImageTexture2D>(filename));
    }
  }
}
