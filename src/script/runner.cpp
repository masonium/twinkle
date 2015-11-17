#include <cassert>
#include <iostream>
#include "vec3.h"
#include "script/runner.h"
#include "script/script_util.h"
#include "util/filesystem.h"

#if FEATURE_LUA_SCRIPTING

using std::cerr;

void lua_deleter::operator()(lua_State* ptr) const
{
  lua_close(ptr);
}


LuaRunner::LuaRunner() : _state(nullptr)
{
}

LuaRunner::LuaRunner(const string& filename) : _state(luaL_newstate())
{
  if (filename.empty())
    return;

  assert(filesystem::exists(filename.c_str()));
  luaL_openlibs(state());

  script::register_all(state());

  auto result = luaL_dofile(state(), filename.c_str());
  if (result != 0)
  {
    auto error = lua_tostring(state(), -1);
    cerr << "Error running " << filename << ":\n";
    cerr << error << std::endl;
    assert(false);
  }
}

spectrum LuaRunner::call_texture_2d_function(const string& fn, const Vec2& v)
{
  auto L = state();
  lua_getglobal(L, fn.c_str());
  lua_pushnumber(L, v[0]);
  lua_pushnumber(L, v[1]);

  lua_pcall(L, 2, 3, 0);

  spectrum s(lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
  lua_pop(L, 3);
  return s;
}

#endif
