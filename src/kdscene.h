#pragma once

#include "scene.h"
#include "kdtree.h"

using std::unique_ptr;

class KDScene : public Scene
{
public:
  KDScene();

  void add(const Shape*) override;
  void add(const Light*) override;

  void prepare() override;

  Light const* sample_light(scalar r1, scalar&) const override;
  EmissionSample sample_emission(Sampler& sampler) const override;

  optional<Intersection> intersect(const Ray& ray, scalar_fp max_t) const override;
  using Scene::intersect;

  spectrum environment_light_emission(const Vec3& dir) const override;

  const vector<const Light*>& lights() const override;

  virtual ~KDScene() {}

private:
  /*
   * temporary storage, used only before the prepare call to store the shapes
   * before they are used to construct the kd tree
  */
  vector<const Shape*> shape_vector_;

  vector<const Shape*> unbounded_shapes_;
  unique_ptr<kd::Tree<Shape>> shape_tree_;

  vector<const Light*> lights_;
  const EnvironmentLight* env_light_;
};
