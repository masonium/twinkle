#pragma once

#include "twinkle.h"
#include <cmath>
#include <memory>
#include <vector>
#include "vector.h"

using std::move;
const scalar PI = 4.0 * atan(1.0);
const scalar INV_PI = 1 / PI;
const scalar INV_2PI = 1 / (2 * PI);
const scalar INV_4PI = 1 / (4 * PI);
const scalar PId4 = PI / 4.0;
const scalar PId2 = PI / 2.0;

#define SCALAR_EPSILON 0.00001

class Vec3;
class Vec2;
class Ray;

scalar approx_gt(scalar x, scalar y);

scalar qf(scalar a, scalar b, scalar c);

template<typename T>
scalar norm(const T& s)
{
  return s.norm();
}

scalar norm(const scalar& s);

inline constexpr scalar square(scalar s)
{
  return s * s;
}

inline constexpr scalar sign(scalar val) {
  return (static_cast<scalar>(0) < val) - (val < static_cast<scalar>(0));
}

/**
 * Return the F0 term in the schlick reflection approximation.
 */
inline constexpr scalar schlick_r0_term(scalar nr)
{
  return square( (nr - 1) / (nr + 1) );
}


/**
 * Computes the Fresnel coefficient for reflectance of dielectric
 * medium. Assumes that incoming and normal originate from the same point.
 */
scalar fresnel_reflectance(const Vec3& incoming, const Vec3& normal,
                           scalar nr);
scalar fresnel_reflectance_schlick(const Vec3& incoming, const Vec3& half,
                           scalar nr);

scalar fresnel_transmittance(const Vec3& incoming, const Vec3& normal,
                             scalar nr);
scalar fresnel_transmittance_schlick(const Vec3& incoming, const Vec3& half,
                                     scalar nr);

bool has_tir(const Vec3& incoming, const Vec3& normal,
             scalar nr);
Vec3 refraction_direction(const Vec3& incoming, const Vec3& normal,
                          scalar nr);

template <typename T>
T lerp(const T& a, const T& b, scalar x)
{
  return a + (b-a) * x;
}

Vec2 to_euler_uv(const Vec3& dir);
Vec3 from_euler_uv(const Vec2& dir);

/**
 * Maps from [0, 1] to [0,1], using a third-order polynomial (First derivative
 * vanishes at 0 and 1).
 */
scalar p3_interp(scalar x);

/**
 * Maps from [0, 1] to [0, 1] using a fifth order polynomial. (First and sceond
 * derivatives vanish at 0 and 1.)
 */
scalar p5_interp(scalar x);

/**
 * Linearly map from [-1, 1] to [0, 1].
 */
scalar unitize(scalar x);

scalar clamp(scalar x, scalar m, scalar M);

Vec3 interpolate_quadratic(scalar x1, scalar y1, scalar x2, scalar y2, scalar x3, scalar y3);

/***
 * Return the convolution of 'data' with a filter. The return vector is of the
 * same size as the data vector. Edge elements are treated as 0.
 */
void convolve(uint w, uint h, const std::vector<scalar>&,
              uint fw, uint fh, const std::vector<scalar>&,
              std::vector<scalar>&);
