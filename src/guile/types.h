#include <cstddef>
#include <libguile.h>
#include "twinkle.h"
#include "vec3.h"

struct scm_vec3
{
  scalar x, y, z;
};

#ifdef USE_DOUBLES
#define scm_to_scalar scm_to_double
#else
#define scm_to_scalar scm_to_float
#endif

Vec3 smob_to_vec3(SCM v);

extern "C"
{
  void init_vec3(void);
}
