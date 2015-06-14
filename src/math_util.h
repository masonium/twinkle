#pragma once

#include "twinkle.h"
#include <cmath>
#include <memory>
#include "vector.h"

using std::move;
const scalar PI = 4.0 * atan(1.0);

#define SCALAR_EPSILON 0.00001

class Vec3;
class Ray;

scalar approx_gt(scalar x, scalar y);

scalar qf(scalar a, scalar b, scalar c);

template<typename T>
scalar norm(const T& s)
{
  return s.norm();
}

scalar norm(const scalar& s);

/**
 * Computes the Fresnel coefficient for reflectance of dielectric
 * medium. Assumes that incoming and normal originate from the same point.
 */
scalar fresnel_reflectance(const Vec3& incoming, const Vec3& normal,
                           scalar n1, scalar n2);

scalar fresnel_transmittance(const Vec3& incoming, const Vec3& normal,
                             scalar n1, scalar n2);

Vec3 refraction_direction(const Vec3& incoming, const Vec3& normal,
                          scalar n1, scalar n2);

template <typename T>
T lerp(const T& a, const T& b, scalar x)
{
  return a + (b-a) * x;
}

/***
 * Intersection of a triangle with a ray
 */
scalar ray_triangle_intersection(const Vec3& v1, const Vec3& v2, const Vec3& v3,
                                 const Ray& ray, scalar max_t = SCALAR_MAX);
