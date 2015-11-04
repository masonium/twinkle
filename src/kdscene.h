#pragma once

#include "scene.h"
#include "kdtree.h"

using std::unique_ptr;

#define FEATURE_KD_SCENE 1
#if FEATURE_KD_SCENE == 1

class KDScene : public Scene
{
public:
  KDScene();

  void add(shared_ptr<const Shape>) override;
  void add(shared_ptr<const Light>) override;
  void add(shared_ptr<const EnvironmentLight>) override;

  void prepare() override;

  Light const* sample_light(scalar r1, scalar&) const override;

  optional<Intersection> intersect(const Ray& ray) const override;

  spectrum environment_light_emission(const Vec3& dir) const override;

  const vector<shared_ptr<const Light>>& lights() const override;

  const shared_ptr<const EnvironmentLight>& env_light() const override
  {
    return env_light_;
  }

  ~KDScene() {}

private:
  /*
   * temporary storage, used only before the prepare call to store the shapes
   * before they are used to construct the kd tree
  */
  vector<shared_ptr<const Shape>> shape_vector_;

  vector<shared_ptr<const Shape>> unbounded_shapes_;
  unique_ptr<kd::Tree<Shape>> shape_tree_;

  vector<shared_ptr<const Light>> lights_;
  shared_ptr<const EnvironmentLight> env_light_;
};

#endif
