#include "integrator.h"

void BWIntegrator::render(Camera* cam, Scene* scene, Film* film)
{
  for (uint y = 0; y < film->height; ++y)
  {
    for (uint x = 0; x < film->width; ++x)
    {
      Ray r = cam->sample_pixel_ray(film, x, y, 0.5, 0.5);
      Intersection isect = scene->intersect(r);
      film->add_sample(x, y, spectrum{scalar(isect.valid() ? 1.0 : 0.0)});
    }
  }
}
