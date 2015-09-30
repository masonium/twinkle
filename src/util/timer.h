#pragma once

#include <chrono>
#include "twinkle.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

class Timer
{
public:
  Timer() : now(high_resolution_clock::now())
  {
  }

  scalar since() const
  {
    return duration_cast<nanoseconds>(high_resolution_clock::now() - now).count() * 1e-9;
  }

private:
  const decltype(high_resolution_clock::now()) now;
};
