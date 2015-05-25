#pragma once

#include <vector>
#include "math_util.h"
#include "shape.h"
#include "light.h"

using std::vector;

class Scene
{
public:
  Scene();

  void add_shape(Shape* shape);
  void add_light(Light* light);

  PossibleEmissive const* sample_emissive(scalar r1, scalar&) const;
  
  Intersection intersect(const Ray& ray) const;
  Intersection shadow_intersect(const Ray& ray) const;
  
private:
  vector<Shape const*> shapes;
  vector<PossibleEmissive const*> lights;

  mutable int em_counter;
};
