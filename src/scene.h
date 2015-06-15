#pragma once

#include <memory>
#include <vector>
#include "math_util.h"
#include "shape.h"
#include "light.h"

using std::vector;
using std::shared_ptr;

class Scene
{
public:
  Scene();

  void add(shared_ptr<const Shape> shape);
  void add(shared_ptr<const Light> light);
  
  Light const* sample_light(scalar r1, scalar&) const;
  
  Intersection intersect(const Ray& ray) const;
  
private:
  vector<shared_ptr<const Shape>> shapes;
  vector<shared_ptr<const Light>> lights;
};
