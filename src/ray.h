#pragma once

#include "vec3.h"

class Ray
{
public:
  Ray(Vec3 pos, Vec3 dir) : position(pos), direction(dir)
  {

  }

  Vec3 evaluate(scalar t) const
  {
    return position + direction * t;
  }
  
  Ray& normalize() 
  {
    direction = direction.normal();
    return *this;
  }

  Ray normal() const
  {
    return Ray{position, direction.normal()};
  }
  
  Ray nudge(scalar eps = SURFACE_EPSILON) const;
  
  Vec3 position, direction;
};

