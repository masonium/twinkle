#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>
#include "triangulate.h"
#include "math_util.h"

using std::cout;
using std::ofstream;

scalar eight_surface(const Vec3& v)
{
  return 4.0 * square(square(v.z)) + 1.0 * (v.x*v.x + v.y*v.y - 4.0 * v.z*v.z);
}

scalar tanglecube(const Vec3& v) // -3 : 3
{
  Vec3 r = v.elem_mult(v);
  return r.dot(r) - 5.0 * v.dot(v) + 11.8;
}

scalar heart(const Vec3& v) // -1.5 : 1.5
{
  scalar a = square(v.x) + 2.25 * square(v.y) + square(v.z) - 1;
  scalar b = v.z*v.z*v.z;
  return a * a * a - (square(v.x) + 9.0/80.0*square(v.y)) * b;

}

scalar f_eval(const Vec3& v)
{
  return v.x *v.x + v.z * v.z - 0.5;
}

int main()
{
  scalar s = 3.0;
  MarchingTetrahedra mt(tanglecube, bounds::AABB(Vec3{-s}, Vec3{s}), 30);
  auto model = mt.model();
  ofstream out("implicit.obj");
  model.save_to_obj(out);
}
