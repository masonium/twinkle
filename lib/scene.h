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

  void add(Shape* shape);
  void add(Light* light);
  
  Light const* sample_light(scalar r1, scalar&) const;
  
  Intersection intersect(const Ray& ray) const;
  Intersection shadow_intersect(const Ray& ray) const;
  
private:
  vector<Shape const*> shapes;
  vector<Light const*> lights;

  mutable int em_counter;
};
