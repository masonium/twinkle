#pragma once

#include "vec3.h"
#include "mat33.h"
#include "ray.h"
#include "bounds.h"

/**
 * Transform represents a 3-dimensional affine transformation.
 */
class Transform
{
public:
  Transform();
  Transform(Mat33 A_, Vec3 b_);

  Ray transform_ray(const Ray& r) const;
  Vec3 transform_point(const Vec3& p) const;
  Vec3 transform_direction(const Vec3& d) const;
  Vec3 transform_normal(const Vec3& n) const;
  bounds::AABB transform_bbox(const bounds::AABB&) const;

  Ray inv_transform_ray(const Ray& r) const;
  Vec3 inv_transform_point(const Vec3& p) const;
  Vec3 inv_transform_direction(const Vec3& d) const;
  Vec3 inv_transform_normal(const Vec3& n) const;

  /**
   * Transformations act to the left. So (A*B).transform*(x) is equivalent to
   * A.transform*( B.transform*(x) )
   */
  Transform operator *(const Transform& rhs) const;

private:
  Transform(Mat33 A_, Vec3 b_, Mat33 Ainv_);

  Mat33 A, Ainv;
  Vec3 b;
};
