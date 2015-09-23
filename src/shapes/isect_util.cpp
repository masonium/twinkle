#include "shapes/isect_util.h"

scalar_fp ray_triangle_intersection(const Ray& ray,
                                   const Vec3& v1, const Vec3& v2, const Vec3& v3,
                                   scalar max_t)
{
  Vec3 e1 = v2 - v1, e2 = v3 - v1;
  Vec3 p = ray.direction.cross(e2);
  scalar det = e1.dot(p);
  if (fabs(det) < EPSILON)
    return scalar_fp::none;

  const scalar inv_det = 1 / det;

  Vec3 corner_to_ray = ray.position - v1;
  Vec3 q = corner_to_ray.cross(e1);

  scalar t = q.dot(e2) * inv_det;
  if (t > max_t || t < EPSILON)
    return scalar_fp::none;

  scalar u = corner_to_ray.dot(p) * inv_det;
  if (u < 0 || u > 1)
    return scalar_fp::none;

  scalar v = q.dot(ray.direction) * inv_det;
  if (v < 0 || u + v > 1)
    return scalar_fp::none;

  return t;
}

