#include "dintegrator.h"
#include "mat33.h"
#include <unordered_map>

#ifdef FEATURE_DEBUG_TRACER

using std::make_pair;

const int NUM_DISTINCT_ID_COLORS = 20;
spectrum color_list[NUM_DISTINCT_ID_COLORS];

DebugIntegrator::DebugIntegrator(DebugIntegrator::Type t) : type(t)
{
  for (int i = 0; i < NUM_DISTINCT_ID_COLORS; ++i)
    color_list[i] = spectrum::from_hsv(i * 18, 0.8, 1.0);
}

void DebugIntegrator::render(const Camera& cam, const Scene& scene, Film& film)
{
  grid_render(*this, cam, scene, film, 0, 4, 1);
}

void DebugIntegrator::render_rect(const Camera& cam, const Scene& scene, Film& film,
                                  const Film::Rect& rect, uint samples_per_pixel) const
{
  ShapeColorMap scm;
  UniformSampler sampler;

  for (uint y = 0; y < rect.height; ++y)
  {
    for (uint x = 0; x < rect.width; ++x)
    {
      auto px = x + rect.x;
      auto py = y + rect.y;
      for (auto d = 0u; d < samples_per_pixel; ++d)
      {
        PixelSample ps = cam.sample_pixel(film.width, film.height, px, py, sampler);

        spectrum s = trace_ray(ps.ray, scene, scm);
        film.add_sample(ps, s);
      }
    }
  }
}

spectrum dir_to_spectrum(const Vec3& dir)
{
   auto c = spectrum{dir.x, dir.y, dir.z};
   return 0.5 * (c + spectrum{0.5});
}

spectrum DebugIntegrator::trace_ray(const Ray& ray, const Scene& scene, ShapeColorMap& scm) const
{
  Intersection isect = scene.intersect(ray);
  
  if (type == DI_ISECT)
    return spectrum{scalar(isect.valid() ? 1.0 : 0.0)};

  if (type == DI_DEPTH)
    return spectrum(1.0 / max<scalar>((isect.position - ray.position).norm(), 1.0));

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

#endif
