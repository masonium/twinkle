#pragma once

#include "twinkle.h"
#include "util/optional.h"
#include "ray.h"

using scalar_fp = forced_positive<scalar>;

const scalar_fp sfp_none = sfp_none;

/***
 * Intersection of a triangle with a ray
 */
scalar_fp ray_triangle_intersection(const Ray& ray,
                                    const Vec3& v1, const Vec3& v2, const Vec3& v3);

scalar_fp ray_triangle_intersection(const Ray& ray,
                                    const Vec3& v1, const Vec3& v2, const Vec3& v3,
                                    scalar_fp max_t);
