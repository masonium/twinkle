#pragma once

#include <functional>
#include "geometry.h"
#include "bounds.h"
#include "vec3.h"

using ImplicitEvalFunc = std::function<scalar(const Vec3&)>;
using ImplicitGradFunc = std::function<Vec3(const Vec3&)>;

ImplicitGradFunc gradient_from_sdf(ImplicitEvalFunc);

class ImplicitSurface : public SimpleGeometry, public Bounded
{

public:
  ImplicitSurface(ImplicitEvalFunc, ImplicitGradFunc,
                  scalar lipschitz_const,
                  const bounds::AABB& box);

  Vec3 normal(SubGeo geo, const Vec3& point) const override;

  scalar_fp intersect(const Ray& r, scalar_fp max_t = sfp_none) const override;

  bounds::AABB get_bounding_box() const override
  {
    return bbox;
  }

private:
  ImplicitEvalFunc f;
  ImplicitGradFunc g;
  scalar L;

  bounds::AABB bbox;
};

shared_ptr<Geometry> make_torus(Vec3 normal, scalar outer_radius, scalar inner_radius);
shared_ptr<Geometry> make_capsule(Vec3 direction, scalar length, scalar radius);
shared_ptr<Geometry> make_rounded_box(Vec3 size, scalar radius);
