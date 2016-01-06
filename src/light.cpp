#include "light.h"
#include "scene.h"

LightSample::LightSample(spectrum s, scalar p, const Vec3& p1, const Vec3& p2) :
  ray{p1, p2-p1}, em(s), p_(p), occ_type(OCCLUSION_POINTS)
{
}

LightSample::LightSample(spectrum s, scalar p, const Ray& ray_) :
  ray(ray_), em(s), p_(p),occ_type(OCCLUSION_RAY)
{
  
}

bool LightSample::is_occluded(const Scene& scene) const
{
  switch (occ_type)
  {
  case OCCLUSION_RAY:
    return scene.intersect(ray).is();
    
  case OCCLUSION_POINTS:
    {
      auto isect = scene.intersect(ray);
      return isect.is() && approx_gt(1.0, isect.get().t());
    }
    
  default:
    return true;
  }
}

std::string PointLight::to_string() const
{
  using std::ostringstream;
  ostringstream out;
  out << "PointLight(" << position << ")";
  return out.str();
}

LightSample PointLight::sample_emission(const Intersection& isect,
                                        Sampler& UNUSED(sampler)) const
{
  // inverse-squared falloff
  const auto d = position - isect.position;
  spectrum emit = _emission;// / d.norm2();

  return LightSample{emit, 1.0, isect.position + d.normal() * 0.001, position};
}

/* Sample an emission generically, from anywhere within the scene. */
EmissionSample PointLight::sample_emission(const Scene& UNUSED(scene), Sampler& sampler) const
{
  const auto d = uniform_sphere_sample(sampler.sample_2d());

  return EmissionSample{this, Ray{position, d},  1/(2.0 * PI)};
}


spectrum PointLight::emission(const Scene& scene, const IntersectionView& isect,
                              const EmissionSample& sample) const
{
  auto L = isect.position - position;
  auto shadow_isect = scene.intersect(Ray{position, L}, scalar_fp{1.0 - EPSILON});
  if (!shadow_isect.is() || shadow_isect.get().t() >= 1.0)
    return _emission;

  return spectrum::zero;
}

LightSample DirectionalLight::sample_emission(const Intersection& isect,
                                              Sampler& UNUSED(sampler)) const
{
  if (isect.normal.dot(direction) < 0)
    return LightSample();

  return LightSample(_emission, 1.0, Ray{isect.position, direction}.normal().nudge(0.0005) );
}

EmissionSample DirectionalLight::sample_emission(const Scene& scene,
                                                 Sampler& UNUSED(sampler)) const
{
  return EmissionSample(this, Ray{-direction * 1000, direction}, 1.0);
}
spectrum DirectionalLight::emission(const Scene& scene, const IntersectionView& isect,
                                          const EmissionSample& e_sample) const
{
  auto shadow_isect = scene.intersect(Ray{isect.position, direction}.nudge(SHADOW_EPSILON));
  return shadow_isect.is() ? spectrum::zero : _emission;
}

