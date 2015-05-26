#include "integrator.h"

void BWIntegrator::render(Camera* cam, Scene* scene, Film* film)
{
  for (uint y = 0; y < film->height; ++y)
  {
    for (uint x = 0; x < film->width; ++x)
    {
      PixelSample ps = cam->sample_pixel(film, x, y, 0.5, 0.5);
      Intersection isect = scene->intersect(ps.ray);
      film->add_sample(ps, spectrum{scalar(isect.valid() ? 1.0 : 0.0)});
    }
  }
}
