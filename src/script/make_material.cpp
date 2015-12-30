#include <lua.hpp>
#include <memory>
#include "material.h"
#include "textures.h"
#include "materials/multilayered.h"
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

    int glossy_paint(lua_State* L)
    {
      LUA_CHECK_NUM_ARGS(L, 1);
      auto tex = lua_totexture(L, 1);

      auto layer = make_shared<MFLayer>(
        0.0, spectrum{1.0}, make_shared<GTR>(0.2));

      vector<decltype(layer)> layers({layer});

      auto base_mat = make_entity<RoughMaterial>(0.0, tex);

      return script_material(L, make_entity<LayeredMFMaterial>(layers, base_mat.get()));
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
