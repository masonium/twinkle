#pragma once
#include <memory>

using std::shared_ptr;

struct lua_State;

class Geometry;
class Material;
class Shape;
class Texture;
class EnvironmentLight;
class Camera;
namespace bounds
{
  class AABB;
}

class Vec3;
class spectrum;

#define LUA_CHECK_NUM_ARGS(L, n)  assert(lua_gettop(L) == n)
#define LUA_CHECK_RANGE_ARGS(L, m, n)  { decltype(m) x = lua_gettop(L); \
    assert(m <= x); \
    assert(x <= n); \
}

namespace script
{
  /*
    The script_* functions take the shared_ptr for an object and construct the
    user_data value for that object, pushing it onto the stack.
   */
  int script_envlight(lua_State*, shared_ptr<EnvironmentLight>);
  int script_geometry(lua_State*, shared_ptr<Geometry>);
  int script_material(lua_State*, shared_ptr<Material>);
  int script_shape(lua_State*, shared_ptr<Shape>);
  int script_texture(lua_State*, shared_ptr<Texture>);
  int script_camera(lua_State*, shared_ptr<Camera>);
  int script_bbox(lua_State*, shared_ptr<::bounds::AABB>);

  /*
    The lua_to* functions act exaclty as the standard lua_to* functions.
  */
  shared_ptr<EnvironmentLight> lua_toenvlight(lua_State*, int);
  shared_ptr<Geometry> lua_togeometry(lua_State*, int);
  shared_ptr<Material> lua_tomaterial(lua_State*, int);
  shared_ptr<Shape> lua_toshape(lua_State*, int);
  shared_ptr<Texture> lua_totexture(lua_State*, int);
  shared_ptr<Camera> lua_tocamera(lua_State*, int);
  shared_ptr<bounds::AABB> lua_tobbox(lua_State*, int);

  int shape(lua_State* L);

  Vec3 lua_tovector(lua_State* L, int index);
  spectrum lua_tospectrum(lua_State* L, int index);

  /**
   * Push the value t[key] to the top of the stack, where t is a table-like value
   * at index.
   */
  void lua_gettablefield(lua_State* L, int index, const char *key);

  /**
   * Return a string representing the type of the object at index.
   */
  std::string lua_typeat(lua_State* L, int index);

  /*
    Register all twinkle-provided methods in the lua state.
   */
  void register_all(lua_State* L);
}
