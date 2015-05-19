#include "integrator.h"

void BWIntegrator::render(Camera* cam, Scene* scene, Film* film)
{
  for (uint y = 0; y < film->height; ++y)
  {
    for (uint x = 0; x < film->width; ++x)
    {
      Ray r = cam->sample_pixel_ray(film, x, y);
      Intersection isect = scene->intersect(r);
      if (isect.valid())
        (*film)(x, y) = static_cast<spectrum>(1);
      else
        (*film)(x, y) = static_cast<spectrum>(0);
    }
  }
}
