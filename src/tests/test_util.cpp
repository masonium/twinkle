#include "test_util.h"

scalar rf()
{
  return rand() / (1.0 + RAND_MAX);
}

// random uniform symmetric
scalar rus()
{
  return rf() * 2.0 - 1.0;
}

Vec3 random_normal()
{
  return Vec3{rus(), rus(), rus()}.normal();
}
