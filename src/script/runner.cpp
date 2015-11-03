#include <cassert>
#include <iostream>
#include "vec3.h"
#include "script/runner.h"
#include "script/make_util.h"
#include "util/filesystem.h"

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
  assert(filesystem::exists(filename.c_str()));
  luaL_openlibs(state());

  script::register_all(state());

  assert(!luaL_dofile(state(), filename.c_str()));
}

spectrum LuaRunner::call_texture_2d_function(const string& fn, const Vec2& v)
{
  auto L = state();
  lua_getglobal(L, fn.c_str());
  lua_pushnumber(L, v[0]);
  lua_pushnumber(L, v[1]);

  lua_call(L, 2, 3);

  spectrum s(lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
  lua_pop(L, 3);
  return s;
}
