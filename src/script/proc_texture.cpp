#include "script/proc_texture.h"
#include "thread_state.h"

#if FEATURE_LUA_SCRIPTING

ScriptTexture::ScriptTexture(const string& fn_name) : _fn_name(fn_name)
{

}

spectrum ScriptTexture::at_coord(const Vec2& uv) const
{
  auto& s = get_thread_lua_runner();

  return s.call_texture_2d_function(_fn_name, uv);
}

#endif
