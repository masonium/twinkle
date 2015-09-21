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
const scalar SCALAR_MIN = std::numeric_limits<scalar>::min();

#define FEATURE_DIRECT_LIGHTING 1
#define FEATURE_PATH_TRACER 1
#define FEATURE_DEBUG_TRACER 1
//#define FEATURE_BIDIRECTIONAL 1
