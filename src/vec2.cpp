#include "vec2.h"
#include <iostream>

using std::ostream;

Vec2& Vec2::operator+=(const Vec2& v)
{
  x += v.x;
  y += v.y;
  return *this;
}

Vec2& Vec2::operator*=(scalar f)
{
  x *= f;
  y *= f;
  return *this;
}

Vec2 operator +(const Vec2& v, const Vec2& w)
{
  Vec2 r(v);
  return r += w;
}

Vec2 operator *(const Vec2& v, scalar f)
{
  return Vec2(v.x * f, v.y * f);
}
Vec2 operator *(scalar f, const Vec2& v)
{
  return Vec2(v.x * f, v.y * f);
}

ostream& operator <<(ostream& o, const Vec2& v)
{
  o << v.x << ' ' << v.y;
  return o;
}
