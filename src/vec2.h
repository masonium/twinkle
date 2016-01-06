#pragma once

#include <iostream>
#include "twinkle.h"

using std::ostream;

class Vec2
{
public:
  Vec2(scalar u_, scalar v_) : x(u_), y(v_) { }
  Vec2(scalar u_) : x(u_), y(u_) { }
  Vec2() : x(0), y(0) { }

  scalar operator [](int i) const {
    return c[i];
  }
  scalar& operator [](int i) {
    return c[i];
  }

  Vec2& operator+=(const Vec2& v);
  Vec2& operator*=(scalar f);

  scalar dot(const Vec2& v);

  Vec2 rotate(scalar angle) const;

  union
  {
    scalar c[2];
    struct
    {
      scalar x, y;
    };
    struct
    {
      scalar u, v;
    };
  };
};

Vec2 operator +(const Vec2& v, const Vec2& w);

Vec2 operator *(const Vec2& v, scalar f);
Vec2 operator *(scalar f, const Vec2& v);

ostream& operator <<(ostream& o, const Vec2& v);
