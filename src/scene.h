#pragma once

#include <memory>
#include <vector>
#include "math_util.h"
#include "shape.h"
#include "light.h"
#include "env_light.h"

using std::vector;
using std::shared_ptr;

class Scene
{
public:
  Scene();

  void add(shared_ptr<const Shape> shape);
  void add(shared_ptr<const Light> light);
  void add(shared_ptr<const EnvironmentalLight>);

  Light const* sample_light(scalar r1, scalar&) const;
  Ray sample_light_ray(Sampler& sample, spectrum& emit) const;

  Intersection intersect(const Ray& ray) const;

  spectrum environment_light_emission(const Vec3& dir) const;

  const vector<shared_ptr<const Light>>& lights() const;
  const shared_ptr<const EnvironmentalLight>& env_light() const
  {
    return env_light_;
  }

private:
  vector<shared_ptr<const Shape>> shapes_;
  vector<shared_ptr<const Light>> lights_;
  shared_ptr<const EnvironmentalLight> env_light_;
};
