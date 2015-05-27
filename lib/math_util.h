#pragma once

#include "twinkle.h"
#include <cmath>
#include <memory>
#include "vector.h"

using std::move;
const scalar PI = 4.0 * atan(1.0);

scalar qf(scalar a, scalar b, scalar c);

template<typename T>
scalar norm(const T& s)
{
  return s.norm();
}

scalar norm(const scalar& s);
