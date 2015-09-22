#pragma once

#include <memory>
#include <vector>
#include "math_util.h"
#include "shape.h"
#include "light.h"
#include "env_light.h"
#include "kdtree.h"

using std::vector;
using std::shared_ptr;

class Scene
{
public:
  Scene() { }

  virtual void add(shared_ptr<const Shape>) = 0;
  virtual void add(shared_ptr<const Light>) = 0;
  virtual void add(shared_ptr<const EnvironmentalLight>) = 0;

  virtual void prepare() { }

  virtual Light const* sample_light(scalar r1, scalar&) const = 0;

  virtual Intersection intersect(const Ray& ray) const = 0;

  virtual spectrum environment_light_emission(const Vec3& dir) const = 0;

  virtual const vector<shared_ptr<const Light>>& lights() const = 0;

  virtual const shared_ptr<const EnvironmentalLight>& env_light() const = 0;

  virtual ~Scene() { }
};

class BasicScene : public Scene
{
public:
  BasicScene();

  void add(shared_ptr<const Shape>) override;
  void add(shared_ptr<const Light>) override;
  void add(shared_ptr<const EnvironmentalLight>) override;

  Light const* sample_light(scalar r1, scalar&) const override;
  Ray sample_light_ray(Sampler& sample, spectrum& emit) const;

  Intersection intersect(const Ray& ray) const override;

  spectrum environment_light_emission(const Vec3& dir) const override;

  const vector<shared_ptr<const Light>>& lights() const override;
  const shared_ptr<const EnvironmentalLight>& env_light() const
  {
    return env_light_;
  }

  ~BasicScene() { }

private:
  vector<shared_ptr<const Shape>> shapes_;
  vector<shared_ptr<const Light>> lights_;
  shared_ptr<const EnvironmentalLight> env_light_;
};
