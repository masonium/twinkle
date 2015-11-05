#pragma once

#include "geometry/implicit.h"
#include <memory>
#include <string>

using std::unique_ptr;

namespace script
{
  class Implicit : public ImplicitSurface
  {
  public:
    Implicit(const std::string& fn_name, scalar lc, const ::bounds::AABB& bbox);

  private:
    scalar eval_func(const Vec3& v);
    Vec3 grad_func(const Vec3&);

    std::string fn_name;
  };

}
