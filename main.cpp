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

  BRDF* light = new EmissiveBRDF{spectrum{4.0}};
  
  // scene.add_shape( new Shape( new Sphere{ Vec3{-1.0, -1.0, 0.5}, 0.5},
  //                             light,
  //                             new SolidColor(spectrum{1.0})) );
  
  scene.add_shape( new Shape( new Sphere{ Vec3{2.0, -1.0, 0.0}, 1.0},
                              b,
                              new SolidColor(spectrum{1.0, 0.2, 0.3})) );
  
  scene.add_shape( new Shape( new Sphere{ Vec3{1.0, 1.0, 0.5}, 0.05},
                              light,
                              new SolidColor(spectrum{1.0})) );
  
  scene.add_shape( new Shape( new Sphere{ Vec3{-2.0, 1.0, 0.0}, 1.0},
                              b,
                              new SolidColor(spectrum{0.2, 1.0, 0.3})) );
  
  scene.add_shape( new Shape( new Sphere{ Vec3{0.0, -1000.0, 0.0}, 998.0},
                              b,
                              new SolidColor(spectrum{0.0, 0.0, 1.0})) );

  const uint WIDTH = 800;
  const uint HEIGHT = 600;

  Film f(WIDTH, HEIGHT);
  Camera cam {Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0, scalar(WIDTH)/HEIGHT};
  
  PathTracerIntegrator igr;

  igr.render(&cam, &scene, &f);
  //f.render_to_console(cout);
  f.render_to_ppm(cout, new BoxFilter, new CutoffToneMapper);
  
  return 0;
}

