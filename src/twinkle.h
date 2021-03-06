#pragma once

#include <limits>
#define USE_DOUBLES

#ifdef USE_DOUBLES

using scalar = double;
const scalar EPSILON         = 0.00001;
const scalar SHADOW_EPSILON  = 0.0001;
const scalar BSDF_PDF_EPSILON = 0.001;

#else

using scalar = float;
const scalar EPSILON         = 0.0001;
const scalar SHADOW_EPSILON  = 0.001;
const scalar BSDF_PDF_EPSILON = 0.001;

#endif

const scalar SCALAR_MAX = std::numeric_limits<scalar>::max();
const scalar SCALAR_MIN = std::numeric_limits<scalar>::min();

#ifndef FEATURE_MULTITHREADED
#define FEATURE_MULTITHREADED 1
#endif

#ifndef FEATURE_LUA_SCRIPTING
#define FEATURE_LUA_SCRIPTING 1
#endif


#ifndef FEATURE_IMAGE_LOADING
#define FEATURE_IMAGE_LOADING 1
#endif

#define FEATURE_DEBUG_TRACER 1
#define FEATURE_DIRECT_LIGHTING 1
#define FEATURE_PATH_TRACER 1
#define FEATURE_BIDIRECTIONAL 1

#define FEATURE_BASIC_SCENE 0
#define FEATURE_KD_SCENE 1
