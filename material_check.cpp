#include "materials/rough_glass_material.h"
#include <iostream>
#include <vector>
#include <memory>

using std::make_shared;
using std::vector;
using std::cout;
using std::endl;

/*
void refl_model_test()
{
  auto mat = make_glass_material(RoughGlassDistribution::GGX, 0.334);
  for (int i = 150; i <= 210; ++i)
  {
    scalar rad = i * PI / 180.0;
  }
}
*/

int main()
{
  GGXDistribution g(0.2);
/*
  for (int angle = -40; angle <= 40; ++angle)
  {
    scalar rad = angle * PI / 180.0;
    Vec3 r(sin(rad), 0, cos(rad));

    cout << angle << " " << g.density(r) << endl;
  }
*/

  ggx_test();
  return 0;
}
