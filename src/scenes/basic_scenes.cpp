#include "basic_scenes.h"
#include "geometries.h"
#include "textures.h"
#include "script/proc_texture.h"
#include "env_light.h"
#include "materials/rough_glass_material.h"
#include "util/timer.h"

using std::shared_ptr;
using std::make_shared;

shared_ptr<Shape> SHAPE(Geometry* x, Material* y)
{
  return make_shared<Shape>(x, y);
}

shared_ptr<Material> COLOR(const spectrum& x)
{
  return make_shared<RoughColorMaterial>(0, x);
}

#if FEATURE_LUA_SCRIPTING
#include "script/runner.h"
#include "script/script_util.h"

Camera* lua_scene(Scene& scene, const string& filename)
{
  LuaRunner runner(filename.c_str());

  auto L = runner.state();
  lua_getglobal(L, "scene");
  assert(lua_isfunction(L, -1));
  lua_call(L, 0, 1);

  // shapes
  script::lua_gettablefield(L, -1, "shapes");
  assert(lua_istable(L, -1));

  int num_shapes = lua_objlen(L, -1);

  for (int i = 0; i < num_shapes; ++i)
  {
    lua_rawgeti(L, -1, i+1);
    auto obj = script::lua_toshape(L, -1);
    lua_pop(L, 1);
    scene.add(obj);
  }

  lua_pop(L, 1);

  // lights
  script::lua_gettablefield(L, -1, "lights");
  assert(lua_istable(L, -1));

  int num_lights = lua_objlen(L, -1);

  for (int i = 0; i < num_lights; ++i)
  {
    lua_rawgeti(L, -1, i+1);
    auto obj = script::lua_tolight(L, -1);
    lua_pop(L, 1);
    scene.add(obj);
  }

  lua_pop(L, 1);

  assert(lua_istable(L, -1));
  script::lua_gettablefield(L, -1, "camera");

  auto camera = script::lua_tocamera(L, -1);
  
  return camera;
}

#endif
