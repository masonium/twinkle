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
  ImplicitSurface(ImplicitEvalFunc, ImplicitGradFunc, scalar lipschitz_const = 1);

  Vec3 normal(SubGeo geo, const Vec3& point) const override;

  scalar intersect(const Ray& r, scalar max_t = SCALAR_MAX) const override;

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
