#pragma once

#include "vec3.h"
#include "mat33.h"
#include "ray.h"

class Transform
{
public:
  Transform(Mat33 A_, Vec3 b_);

  Ray transform_ray(const Ray& r) const;
  Vec3 transform_point(const Vec3& p) const;
  Vec3 transform_direction(const Vec3& d) const;
  Vec3 transform_normal(const Vec3& n) const;

  Ray inv_transform_ray(const Ray& r) const;
  Vec3 inv_transform_point(const Vec3& p) const;
  Vec3 inv_transform_direction(const Vec3& d) const;
  Vec3 inv_transform_normal(const Vec3& n) const;


private:
  Mat33 A, Ainv;
  Vec3 b;
};
