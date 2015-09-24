#include "shapes/isect_util.h"

scalar_fp ray_triangle_intersection(const Ray& ray,
                                    const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
  return ray_triangle_intersection(ray, v1, v2, v3, sfp_none);
}


scalar_fp ray_triangle_intersection(const Ray& ray,
                                   const Vec3& v1, const Vec3& v2, const Vec3& v3,
                                   scalar_fp max_t)
{
  Vec3 e1 = v2 - v1, e2 = v3 - v1;
  Vec3 p = ray.direction.cross(e2);
  scalar det = e1.dot(p);
  if (fabs(det) < EPSILON)
    return sfp_none;

  const scalar inv_det = 1 / det;

  Vec3 corner_to_ray = ray.position - v1;
  Vec3 q = corner_to_ray.cross(e1);

  scalar t = q.dot(e2) * inv_det;
  if (max_t < t || t < EPSILON)
    return sfp_none;

  scalar u = corner_to_ray.dot(p) * inv_det;
  if (u < 0 || u > 1)
    return sfp_none;

  scalar v = q.dot(ray.direction) * inv_det;
  if (v < 0 || u + v > 1)
    return sfp_none;

  return scalar_fp{t};
}

