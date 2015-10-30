#pragma once
#include <memory>

using std::shared_ptr;

struct lua_State;

class Geometry;
class Material;
class Shape;
class Texture;
class EnvironmentalLight;
class Camera;

class Vec3;
class spectrum;

#define LUA_CHECK_NUM_ARGS(L, n)  assert(lua_gettop(L) == n)
#define LUA_CHECK_RNAGE_ARGS(L, m, n)  { int x = lua_gettop(L); \
    assert(m <= x); \
    assert(x <= n); \
}

namespace script
{
  /*
    The script_* functions take the shared_ptr for an object and construct the
    user_data value for that object, pushing it onto the stack.
   */
  int script_envlight(lua_State*, shared_ptr<EnvironmentalLight>);
  int script_geometry(lua_State*, shared_ptr<Geometry>);
  int script_material(lua_State*, shared_ptr<Material>);
  int script_shape(lua_State*, shared_ptr<Shape>);
  int script_texture(lua_State*, shared_ptr<Texture>);
  int script_camera(lua_State*, shared_ptr<Camera>);


  /*
    The lua_to* functions act exaclty as the standard lua_to* functions.
  */
  shared_ptr<EnvironmentalLight> lua_toenvlight(lua_State*, int);
  shared_ptr<Geometry> lua_togeometry(lua_State*, int);
  shared_ptr<Material> lua_tomaterial(lua_State*, int);
  shared_ptr<Shape> lua_toshape(lua_State*, int);
  shared_ptr<Texture> lua_totexture(lua_State*, int);
  shared_ptr<Camera> lua_tocamera(lua_State*, int);

  int shape(lua_State* L);

  Vec3 lua_tovector(lua_State* L, int index);
  spectrum lua_tospectrum(lua_State* L, int index);

  /*
    Register all twinkle-provided methods in the lua state.
   */
  void register_all(lua_State* L);
}
