#include <cassert>
#include <script/runner.h>
#include <util/filesystem.h>

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
  luaL_openlibs(_state.get());
  assert(!luaL_dofile(_state.get(), filename.c_str()));
}

spectrum LuaRunner::call_texture_2d_function(const string& fn, const Vec2& v)
{
  auto L = _state.get();
  lua_getglobal(L, fn.c_str());
  lua_pushnumber(L, v[0]);
  lua_pushnumber(L, v[1]);

  lua_call(L, 2, 3);

  spectrum s(lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
  lua_pop(L, 3);
  return s;
}
