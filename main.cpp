#include <iostream>

#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "bsdf.h"
#include "integrator.h"

using std::cout;

void smallpt_scene(Scene& scene)
{
  BRDF* diffuse = new Diffuse{1.0};
  BRDF* light = new EmissiveBRDF{spectrum{12.0}};
  scene.add_shape(new Shape(new Sphere{ Vec3{1e5+1, 40.8, 81.6}, 1e5 },
                      diffuse,
                      new SolidColor(spectrum{0.75,0.25,0.25})));
  scene.add_shape(new Shape(new Sphere{ Vec3{-1e5+99, 40.8, 81.6}, 1e5 },
                      diffuse,
                      new SolidColor(spectrum{0.25, 0.25, 0.75})));
  scene.add_shape(new Shape(new Sphere{Vec3{50, 40.8, 1e5}, 1e5},
                      diffuse,
                      new SolidColor(spectrum{0.75})));
  scene.add_shape(new Shape(new Sphere{Vec3{50, 1e5, 81.6}, 1e5},
                      diffuse,
                      new SolidColor(spectrum{0.75})));
  scene.add_shape(new Shape(new Sphere{Vec3{50, -1e5+81.6, 81.6}, 1e5},
                      diffuse,
                      new SolidColor(spectrum{0.75})));

  scene.add_shape(new Shape(new Sphere{Vec3{50,681.6-.27,81.6}, 600},
                      light,
                      new SolidColor(spectrum{1.0})));
}

int main(int argc, char** args)
{
  Scene scene;
  BRDF* b = new Diffuse{1.0};

  BRDF* light = new EmissiveBRDF{spectrum{4.0}};
  BRDF* mirror = new PerfectMirrorBRDF{};

  //smallpt_scene(scene);
  
  scene.add_shape( new Shape( new Sphere{ Vec3{-2.0, -1.0, 0.0}, 1.0},
                              mirror,
                              new SolidColor(spectrum{1.0})) );
  
  scene.add_shape( new Shape( new Sphere{ Vec3{2.0, -1.0, 0.0}, 1.0},
                              b,
                              new SolidColor(spectrum{1.0, 0.2, 0.3})) );
  
  scene.add_shape( new Shape( new Sphere{ Vec3{-2.0, 1.0, 0.0}, 1.0},
                              b,
                              new SolidColor(spectrum{0.2, 1.0, 0.3})) );
  
  scene.add_shape( new Shape( new Sphere{ Vec3{0.0, -1000.0, 0.0}, 998.0},
                              b,
                              new SolidColor(spectrum{0.1, 0.2, 0.9})));
  
  scene.add_shape( new Shape( new Sphere{ Vec3{0.0, 3.0, 1.0}, 0.05},
                              light,
                              new SolidColor(spectrum{1.0})) );

  const uint WIDTH = 800;
  const uint HEIGHT = 600;

  Film f(WIDTH, HEIGHT);
  Camera cam {Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0}, PI / 2.0,
  scalar(WIDTH)/HEIGHT};
  // Vec3 pos{50, 52, 295.6};
  // Vec3 dir = Vec3{0, -0.042612, -1}.normal();
  // Camera cam{ pos, pos+dir, Vec3{0, 1, 0}, PI/2.0, scalar(WIDTH)/HEIGHT};
  
  PathTracerIntegrator igr;

  igr.render(&cam, &scene, &f);
  //f.render_to_console(cout);
  f.render_to_ppm(cout, new BoxFilter, new LinearToneMapper);
  
  return 0;
}

