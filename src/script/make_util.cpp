#include <lua.hpp>
#include <iostream>
#include <cassert>
#include "util/manager.h"
#include "script/make_util.h"
#include "vec3.h"
#include "spectrum.h"
#include "shape.h"
#include "script/make_geometry.h"
#include "script/make_material.h"

using std::cerr;

namespace script
{
  template <typename T>
  int script_ptr(lua_State* L, shared_ptr<T> obj)
  {
    auto ret = Manager<T>::instance().save(obj);
    auto user_data = lua_newuserdata(L, sizeof(ret));
    *reinterpret_cast<decltype(ret)*>(user_data) = ret;
    return 1;
  }

  template <typename T>
  shared_ptr<T> lua_toobj(lua_State* L, int index)
  {
    assert(lua_isuserdata(L, index));

    auto mgr = Manager<T>::instance();
    using key_t = typename Manager<T>::key_type;
    key_t* user_data_key = reinterpret_cast<key_t*>(lua_touserdata(L, index));
    return mgr.at(*user_data_key);
  }

  int script_geometry(lua_State* L , shared_ptr<Geometry> obj)
  {
    return script_ptr(L, obj);
  }
  int script_material(lua_State* L, shared_ptr<Material> obj)
  {
    return script_ptr(L, obj);
  }

  int script_shape(lua_State* L, shared_ptr<Shape> obj)
  {
    return script_ptr(L, obj);
  }

  int script_texture(lua_State* L, shared_ptr<Texture> obj)
  {
    return script_ptr(L, obj);
  }

  int script_envlight(lua_State* L, shared_ptr<EnvironmentalLight> obj)
  {
    return script_ptr(L, obj);
  }

  shared_ptr<EnvironmentalLight> lua_toenvlight(lua_State* L, int index)
  {
    return lua_toobj<EnvironmentalLight>(L, index);
  }
  shared_ptr<Geometry> lua_togeometry(lua_State* L, int index)
  {
    return lua_toobj<Geometry>(L, index);
  }
  shared_ptr<Material> lua_tomaterial(lua_State* L, int index)
  {
    return lua_toobj<Material>(L, index);
  }
  shared_ptr<Shape> lua_toshape(lua_State* L, int index)
  {
    return lua_toobj<Shape>(L, index);
  }
  shared_ptr<Texture> lua_totexture(lua_State* L, int index)
  {
    return lua_toobj<Texture>(L, index);
  }

  Vec3 lua_tovector(lua_State* L, int index)
  {
    if (lua_istable(L, index))
    {
      Vec3 v;
      for (auto i = 0u; i < 3; ++i)
      {
        lua_rawgeti(L, index - i, i+1);
        v[i] = lua_tonumber(L, -1);
      }
      lua_pop(L, 3);
      return v;
    }
    else
    {
      cerr << "value must be a table to convert to a vector\n";
      exit(1);
    }
  }

  spectrum lua_tospectrum(lua_State* L, int index)
  {
    if (lua_istable(L, index))
    {
      spectrum s;
      for (auto i = 0u; i < 3; ++i)
      {
        lua_rawgeti(L, index -i, i+1);
        s[i] = lua_tonumber(L, -1);
      }
      lua_pop(L, 3);
      return s;
    }
    else
    {
      cerr << "value must be a table to convert to a spectrum\n";
      exit(1);
    }
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
    lua_register(L, "sphere", sphere);
    lua_register(L, "plane", plane);
    lua_register(L, "material_color", color_material);
    lua_register(L, "envlight_color", color_env_light);
    lua_register(L, "shape", shape);
  }
}
