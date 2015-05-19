#include <iostream>

#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "bsdf.h"
#include "integrator.h"

using std::cout;

int main(int argc, char** args)
{
  Scene scene;
  BRDF* b = new Diffuse{1.0};

  scene.add_shape( new Shape{ new Sphere{ Vec3{-1.0, 0.0, 0.0}, 0.5}, b} );
  scene.add_shape( new Shape{ new Sphere{ Vec3{2.0, 0.0, 0.0}, 1.0}, b} );

  Film f(40, 40);
  Camera cam {Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0, 1.0};
  
  BWIntegrator igr;

  igr.render(&cam, &scene, &f);
  f.render(cout);
  
  return 0;
}

