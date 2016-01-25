#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>
#include "triangulate.h"
#include "math_util.h"

using std::cout;
using std::ofstream;

scalar f_eval(const Vec3& v)
{
  return v.norm2() - square(0.9);
}

int main()
{
  //Vec3 base = Vec3{0.0, 0.0, 0.0};
  Vec3 base = Vec3{-1.0};
  MarchingTetrahedra mt(f_eval, bounds::AABB(base, base + Vec3{2.0}), 10);
  auto model = mt.model();
  ofstream out("implicit.obj");
  model.save_to_obj(out);
}
