#include "light.h"
#include "scene.h"

LightSample::LightSample(spectrum s, const Vec3& p1, const Vec3& p2) :
  ray{p1, p2-p1}, em(s), occ_type(OCCLUSION_POINTS)
{
}

LightSample::LightSample(spectrum s, const Ray& ray_) :
  ray(ray_), em(s), occ_type(OCCLUSION_RAY)
{
  
}

bool LightSample::is_occluded(const Scene* scene) const
{
  switch (occ_type)
  {
  case OCCLUSION_RAY:
    return scene->intersect(ray);
    
  case OCCLUSION_POINTS:
    {
      auto isect = scene->intersect(ray);
      return isect.valid() && approx_gt(1.0, isect.t);
    }
    
  default:
    return true;
  }
}


LightSample PointLight::sample_emission(const Intersection& isect,
                                        scalar r1, scalar r2) const
{
  // inverse-squared falloff
  const auto d = position - isect.position;
  spectrum emit = emission;// / d.norm2();

  return LightSample{emit, isect.position + d.normal() * 0.001, position};
}

LightSample DirectionalLight::sample_emission(const Intersection& isect,
                                              scalar r1, scalar r2) const
{
  if (isect.normal.dot(direction) < 0)
    return LightSample();

  return LightSample(emission, Ray{isect.position, direction}.normal().nudge(0.0005) );
}