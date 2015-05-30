#include "math_util.h"
#include "vec3.h"
#include <iostream>
#include "bsdf.h"

int main( int argc, char** args )
{
  const int N = 20;
  for (int ai = 0; ai < N; ++ai)
  {
    scalar angle = ai / 20.0 * PI / 2;

    std::cout << angle * (180 / PI) << ": " << fresnel_reflectance(Vec3(0, 0, 1), Vec3::from_euler(0, angle),
                                                              refraction_index::AIR, refraction_index::CROWN_GLASS)
              << "\n";
  }
}
