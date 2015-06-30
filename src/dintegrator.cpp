#include "dintegrator.h"
#include "mat33.h"
#include <unordered_map>

using std::make_pair;

const int NUM_DISTINCT_ID_COLORS = 20;
spectrum color_list[NUM_DISTINCT_ID_COLORS];

DebugIntegrator::DebugIntegrator(DebugIntegrator::Type t) : type(t)
{
  for (int i = 0; i < NUM_DISTINCT_ID_COLORS; ++i)
    color_list[i] = spectrum::from_hsv(i * 18, 0.8, 1.0);
}

void DebugIntegrator::render(const Camera* cam, const Scene* scene, Film& film)
{
  ShapeColorMap scm;
  ConstSampler sampler(0.5, 0.5);

  for (uint y = 0; y < film.height; ++y)
  {
    for (uint x = 0; x < film.width; ++x)
    {
      PixelSample ps = cam->sample_pixel(film, x, y, sampler);

      spectrum s = trace_ray(ps.ray, scene, scm);
      film.add_sample(ps, s);
    }
  }
}

spectrum dir_to_spectrum(const Vec3& dir)
{
   auto c = spectrum{dir.x, dir.y, dir.z};
   return 0.5 * (c + spectrum{0.5});
}

spectrum DebugIntegrator::trace_ray(const Ray& ray, const Scene* scene, ShapeColorMap& scm)
{
  Intersection isect = scene->intersect(ray);
  
  if (type == DI_ISECT)
    return spectrum{scalar(isect.valid() ? 1.0 : 0.0)};

  if (type == DI_OBJECT_ID)
  {
    if (!isect.valid())
      return spectrum::zero;
    
    auto shape = isect.get_shape_for_id();
    if (scm.find(shape) == scm.end())
      scm.insert(make_pair(shape, color_list[scm.size() % NUM_DISTINCT_ID_COLORS]));

    return scm[shape];
  }

  if (type == DI_NORMAL)
  {
    if (!isect.valid())
      return spectrum::zero;
    return dir_to_spectrum(isect.normal);
  }

  if (type == DI_SPECULAR)
  {
    if (!isect.valid())
      return spectrum::zero;

    scalar brdf_p, reflectance;
    ConstSampler s{1.0, 0.0};
    auto dir = isect.sample_bsdf(-ray.direction.normal(), s, brdf_p, reflectance);
    return dir_to_spectrum(dir);
  }

  return spectrum::zero;
}
