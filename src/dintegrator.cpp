#include "dintegrator.h"

using std::make_pair;

DebugIntegrator::DebugIntegrator(DebugIntegrator::Type t) : type(t)
{
}

void DebugIntegrator::render(const Camera* cam, const Scene* scene, Film& film)
{
  for (uint y = 0; y < film.height; ++y)
  {
    for (uint x = 0; x < film.width; ++x)
    {
      PixelSample ps = cam->sample_pixel(film, x, y, Sample5D{{0.5, 0.5, 0, 0, 0}});

      spectrum s = trace_ray(ps.ray, scene);
      film.add_sample(ps, s);
    }
  }
}

spectrum DebugIntegrator::trace_ray(const Ray& ray, const Scene* scene)
{
  Intersection isect = scene->intersect(ray);
  
  if (type == DI_ISECT || type == DI_OBJECT_ID)
    return  spectrum{scalar(isect.valid() ? 1.0 : 0.0)};

  if (type == DI_SPECULAR)
  {
    if (!isect.valid())
      return spectrum::zero;

    scalar brdf_p, reflectance;
    auto dir = isect.sample_bsdf(-ray.direction.normal(), Sample2D{1.0, 0}, brdf_p, reflectance);
    scalar L = std::max<scalar>(dir.norm2(), 0.0);
    return spectrum{L};
  }

  return spectrum::zero;
}
