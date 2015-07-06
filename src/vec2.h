#pragma once

#include "twinkle.h"

class Vec2
{
public:
  Vec2(scalar u_, scalar v_) : x(u_), y(v_) { }
  Vec2(scalar u_) : x(u_), y(u_) { }
  Vec2() : x(0), y(0) { }

  scalar operator [](int i) const {
    return v[i];
  }
  scalar& operator [](int i) {
    return v[i];
  }

  union
  {
    scalar v[2];
    struct
    {
      scalar x, y;
    };
  };
};
