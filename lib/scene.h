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
  
  Intersection&& intersect(const Ray& ray) const;
  
private:
  vector<Shape const*> shapes;
  vector<Light*> lights;
};
