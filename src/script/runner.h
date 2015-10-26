#pragma once

#include <lua.hpp>
#include <memory>
#include <vec2.h>
#include <spectrum.h>

using std::unique_ptr;

struct lua_deleter
{
  void operator()(lua_State* ptr) const;
};

// instantiation of lua state
class LuaRunner
{
public:
  LuaRunner();
  LuaRunner(const string& filename);

  spectrum call_texture_2d_function(const string&, const Vec2& coord);

  lua_State* state() { return _state.get(); }

private:
  unique_ptr<lua_State, lua_deleter> _state;
};
