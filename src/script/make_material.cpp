#include <lua.hpp>
#include <memory>
#include "material.h"
#include "textures.h"
#include "materials/experimental.h"
#include "materials/rough_mirror.h"
#include "materials/rough_glass_material.h"
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

    int dielectric(lua_State* L)
    {
      LUA_CHECK_RANGE_ARGS(L, 0, 2);

      int i = 0;
      scalar ref_in = refraction_index::CROWN_GLASS;
      scalar ref_out = refraction_index::AIR;
      if (lua_gettop(L) > 0)
      {
        ref_in = lua_tonumber(L, i++);
        if (lua_gettop(L) > 1)
          ref_out = lua_tonumber(L, i++);
      }

      return script_material(L, make_shared<GlassMaterial>(ref_in, ref_out));
    }

    int rough_dielectric(lua_State* L)
    {
      LUA_CHECK_RANGE_ARGS(L, 1, 3);

      int i = 1;
      scalar roughness = lua_tonumber(L, i++);
      scalar ref_in = refraction_index::CROWN_GLASS;
      scalar ref_out = refraction_index::AIR;
      if (lua_gettop(L) > 1)
      {
        ref_in = lua_tonumber(L, i++);
        if (lua_gettop(L) > 1)
          ref_out = lua_tonumber(L, i++);
      }

      return script_material(L, make_glass_material(roughness, ref_in, ref_out));
    }

    int glossy_paint(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 1);
      auto tex = lua_totexture(L, 1);

      return script_material(L, make_entity<SimpleSmoothPlastic>(tex, 0.0));
    }

    int rough_mirror(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 1);
      auto r = lua_tonumber(L, 1);
      assert(r > 0);

      return script_material(L, make_entity<RoughMirror>(r));
    }

    int diffuse(lua_State* L)
    {
      LUA_CHECK_RANGE_ARGS(L, 1, 2);
      auto tex = lua_totexture(L, 1);
      scalar roughness = 0.0;
      if (lua_gettop(L) == 2)
        roughness = lua_tonumber(L, 2);

      return script_material(L, make_entity<RoughMaterial>(roughness, tex));
    }

    int emissive(lua_State* L)
    {
      LUA_CHECK_RANGE_ARGS(L, 1, 2);
      auto tex = lua_totexture(L, 1);

      return script_material(L, make_entity<EmissiveMaterial>(tex));
    }

    int mirror(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 0);
      return script_material(L, make_shared<MirrorMaterial>());
    }
  }
}
