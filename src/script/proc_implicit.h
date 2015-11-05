#pragma once

#include "geometry/implicit.h"
#include <string>

class ScriptImplicitFuncs
{
public:
  ScriptImplicitFuncs(const std::string& fn_name);

private:
  scalar eval_func(const Vec3& v);
};
