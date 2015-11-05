#include <lua.hpp>
#include <iostream>
#include <cassert>
#include "util/manager.h"
#include "script/script_util.h"
#include "vec3.h"
#include "spectrum.h"
#include "shape.h"
#include "script/make_geometry.h"
#include "script/make_material.h"
#include "script/make_scene.h"

using std::cerr;
using std::make_shared;

namespace script
{
  template <typename T>
  int script_ptr(lua_State* L, shared_ptr<T> obj)
  {
    auto& mgr = Manager<T>::instance();
    auto ret = mgr.save(obj);
    auto user_data = lua_newuserdata(L, sizeof(ret));
    *reinterpret_cast<decltype(ret)*>(user_data) = ret;
    return 1;
  }

  template <typename T>
  shared_ptr<T> lua_toobj(lua_State* L, int index)
  {
    assert(lua_isuserdata(L, index));

    auto& mgr = Manager<T>::instance();
    using key_t = typename Manager<T>::key_type;
    key_t* user_data_key = reinterpret_cast<key_t*>(lua_touserdata(L, index));
    return mgr.at(*user_data_key);
  }

/*
 * define the lua pusher/popper for a given type
 */
#define LUA_METHODS(CLASSNAME, SHORTNAME) \
  int script_##SHORTNAME(lua_State* L, shared_ptr<CLASSNAME> obj) \
  { \
    return script_ptr(L, obj); \
  } \
  shared_ptr<CLASSNAME> lua_to##SHORTNAME(lua_State* L, int index) \
  { \
    return lua_toobj<CLASSNAME>(L, index); \
  }

  LUA_METHODS(Geometry, geometry);
  LUA_METHODS(EnvironmentLight, envlight);
  LUA_METHODS(Material, material);
  LUA_METHODS(Shape, shape);
  LUA_METHODS(Texture, texture);
  LUA_METHODS(Camera, camera);
  LUA_METHODS(::bounds::AABB, bbox);

  template <typename T, typename Tag>
  T lua_tovtype(lua_State* L, int index, Tag tag)
  {
    if (lua_istable(L, index))
    {
      // translate the index to be positive
      int pos_index = index > 0 ? index : (lua_gettop(L) + index + 1);
      T v;

      // construct the vector based on the table length
      auto tlen = lua_objlen(L, pos_index);

      if (tlen == 3)
      {
        for (auto i = 0u; i < 3; ++i)
        {
          lua_rawgeti(L, pos_index, i+1);
          v[i] = lua_tonumber(L, -1);
        }
        lua_pop(L, 3);
      }
      else if (tlen == 1)
      {
        lua_rawgeti(L, pos_index, 1);
        v[0] = v[1] = v[2] = lua_tonumber(L, -1);
        lua_pop(L, 1);
      }
      else
      {
        assert("lua_tovtype invalid for tables not of length 1 or 3" && false);
      }
      return v;
    }
    else
    {
      cerr << "value must be a table to convert to a vector\n";
      exit(1);
    }
  }

  struct lua_tovector_tag {};
  Vec3 lua_tovector(lua_State* L, int index)
  {
    lua_tovector_tag ltt;
    return lua_tovtype<Vec3, lua_tovector_tag>(L, index, ltt);
  }

  struct lua_tospectrum_tag {};
  spectrum lua_tospectrum(lua_State* L, int index)
  {
    lua_tospectrum_tag ltt;
    return lua_tovtype<spectrum, lua_tospectrum_tag>(L, index, ltt);
  }

  void lua_gettablefield(lua_State* L, int index, const char *key)
  {
    lua_pushvalue(L, index);
    assert(lua_istable(L, -1));
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    lua_remove(L, -2);
  }

  std::string lua_typeat(lua_State* L, int index)
  {
    return std::move(std::string(lua_typename(L, lua_type(L, index))));
  }


  int shape(lua_State* L)
  {
    LUA_CHECK_NUM_ARGS(L, 2);

    auto geo = lua_togeometry(L, 1);
    auto mat = lua_tomaterial(L, 2);

    auto shape = make_shared<Shape>(geo, mat);
    return script_shape(L, shape);
  }

  void register_all(lua_State* L)
  {
    luaL_Reg geom_package[] = {{"sphere", geometry::sphere},
                               {"plane", geometry::plane},
                               {NULL, NULL}};
    luaL_register(L, "geom", geom_package);

    luaL_Reg material_package[] = {{"color", material::color},
                                   {"mirror", material::mirror},
                                   {NULL, NULL}};
    luaL_register(L, "material", material_package);

    lua_register(L, "envlight_color", color_env_light);

    luaL_Reg camera_package[] = {{"spherical", camera::spherical},
                                 {"perspective", camera::perspective},
                                 {NULL, NULL}};

    luaL_register(L, "camera", camera_package);

    lua_register(L, "shape", shape);
  }
}
