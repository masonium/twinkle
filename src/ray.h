#pragma once

#include "vec3.h"

class Ray
{
public:
  Ray(Vec3 pos, Vec3 dir) 
    : position(pos), direction(dir), inv_direction{1/dir.x, 1/dir.y, 1/dir.z},
      sign{inv_direction.x < 0, inv_direction.y < 0, inv_direction.z < 0}
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
  Vec3 inv_direction;
  int8_t sign[3];
};

