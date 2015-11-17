#include "thread_state.h"
#include "script/proc_implicit.h"
#include "script/script_util.h"
#include <lua.hpp>

#if FEATURE_LUA_SCRIPTING

using std::make_unique;

namespace script
{
  Implicit::Implicit(const std::string& fn_name_, scalar lc, const bounds::AABB& bbox)
    : ImplicitSurface(
      [=] (const Vec3& v)
      {
        return this->eval_func(v);
      },
      [=] (const Vec3& v)
      {
        return this->grad_func(v);
      },
      lc,
      bbox
      ), fn_name(fn_name_)
  {
  }
  scalar lua_eval(lua_State* L, const char* fn, scalar x, scalar y, scalar z)
  {
    lua_getglobal(L, fn);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, z);

    lua_call(L, 3, 1);

    scalar ret = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return ret;
  }

  scalar Implicit::eval_func(const Vec3& v)
  {
    auto& runner = get_thread_lua_runner();
    auto L = runner.state();

    return lua_eval(L, fn_name.c_str(), v[0], v[1], v[2]);
  }

  Vec3 Implicit::grad_func(const Vec3& v)
  {
    auto& runner = get_thread_lua_runner();
    auto L = runner.state();

    const auto cfn = fn_name.c_str();

    const scalar GRAD_STEP = 0.0001;
    return Vec3(lua_eval(L, cfn, v.x+GRAD_STEP, v.y, v.z),
                lua_eval(L, cfn, v.x, v.y + GRAD_STEP, v.z),
                lua_eval(L, cfn, v.x, v.y, v.z + GRAD_STEP));
  }

////////////////////////////////////////////////////////////////////////////////


  ImplicitWithGrad::ImplicitWithGrad(const std::string& fn_name_, const std::string& grad_name_,
                                     scalar lc, const bounds::AABB& bbox)
    : ImplicitSurface(
      [=] (const Vec3& v)
      {
        return this->eval_func(v);
      },
      [=] (const Vec3& v)
      {
        return this->grad_func(v);
      },
      lc,
      bbox
      ), fn_name(fn_name_), grad_name(grad_name_), fn(fn_name.c_str()), grad(grad_name.c_str())
  {
  }

  scalar ImplicitWithGrad::eval_func(const Vec3& v)
  {
    auto& runner = get_thread_lua_runner();
    auto L = runner.state();

    return lua_eval(L, fn, v[0], v[1], v[2]);
  }

  Vec3 lua_grad_eval(lua_State* L, const char* fn, scalar x, scalar y, scalar z)
  {
    lua_getglobal(L, fn);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, z);

    lua_call(L, 3, 3);

    Vec3 ret(lua_tonumber(L, -1), lua_tonumber(L, -2), lua_tonumber(L, -3));
    lua_pop(L, 3);
    return ret;
  }

  Vec3 ImplicitWithGrad::grad_func(const Vec3& v)
  {
    auto& runner = get_thread_lua_runner();
    auto L = runner.state();

    return lua_grad_eval(L, grad, v[0], v[1], v[2]);
  }

}

#endif
