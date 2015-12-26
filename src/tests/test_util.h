#include <memory>

#include "unittest++/UnitTest++.h"
#include "twinkle.h"
#include "vec3.h"

using namespace UnitTest;


#define EPS 0.0001
#define PRECISE_EPS 0.00001
#define CHECK_VEC(x, y) CHECK_ARRAY_CLOSE((x).v, (y).v, 3, EPS)
#define CHECK_VEC2(x, y) CHECK_ARRAY_CLOSE((x).c, (y).c, 2, EPS)
#define CHECK_MAT(x, y) CHECK_ARRAY_CLOSE((x).v, (y).v, 9, EPS)

using std::shared_ptr;
using std::make_shared;

// random uniform
scalar rf();

// random symmetric uniform
scalar rus();

// random normal vector
Vec3 random_normal();
