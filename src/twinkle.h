#pragma once

#include <limits>

#ifdef USE_DOUBLES

using scalar = double;
const scalar EPSILON         = 0.0001;

#else

using scalar = float;
const scalar EPSILON         = 0.00001;

#endif

const scalar SCALAR_MAX = std::numeric_limits<scalar>::max();



