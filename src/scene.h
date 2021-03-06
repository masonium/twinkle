#pragma once

#include <memory>
#include <vector>
#include "shape.h"
#include "light.h"
#include "env_light.h"
#include "util/optional.h"

using std::vector;
using std::shared_ptr;

class Scene
{
public:
  Scene() { }

  virtual void add(const Shape*) = 0;
  virtual void add(const Light*) = 0;

  virtual void prepare() { }

  virtual Light const* sample_light(scalar r1, scalar&) const = 0;
  virtual EmissionSample sample_emission(Sampler& sampler) const = 0;

  virtual optional<Intersection> intersect(const Ray& ray, scalar_fp max_t) const = 0;
  virtual optional<Intersection> intersect(const Ray& ray) const
  {
    return intersect(ray, sfp_none);
  }

  virtual spectrum environment_light_emission(const Vec3& dir) const = 0;

  virtual const vector<const Light*>& lights() const = 0;

  virtual ~Scene() { }
};

shared_ptr<Scene> make_scene(const std::string& scene_type);

#if FEATURE_BASIC_SCENE

class BasicScene : public Scene
{
public:
  BasicScene();

  void add(const Shape*) override;
  void add(const Light*) override;

  Light const* sample_light(scalar r1, scalar&) const override;
  Ray sample_light_ray(Sampler& sample, spectrum& emit) const;

  using Scene::intersect;
  optional<Intersection> intersect(const Ray& ray) const override;

  spectrum environment_light_emission(const Vec3& dir) const override;

  const vector<const Light*>& lights() const override;

  virtual ~BasicScene() { }

private:
  vector<const Shape*> shapes_;
  vector<const Light*> lights_;
  vector<const EnvironmentLight*> env_lights_;
};

#endif
