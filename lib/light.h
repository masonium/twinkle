#pragma once

#include "spectrum.h"
#include "texture.h"
#include "shape.h"

class Scene;

enum OcclusionType
{
  OCCLUSION_RAY,
  OCCLUSION_POINTS,
  OCCLUSION_NONE
};

class LightSample
{
public:
  LightSample() : ray{Vec3::zero, Vec3::zero}, occ_type(OCCLUSION_NONE) {}
  LightSample(spectrum s, const Vec3& p1, const Vec3& p2 );
  LightSample(spectrum s, const Ray& r);


  operator bool() const
  {
    return occ_type != OCCLUSION_NONE;
  }

  Vec3 direction( ) const { return ray.direction.normal(); }

  bool is_occluded( const Scene* scene ) const;

  spectrum emission() const { return em; }

private:
  Ray ray;
  spectrum em;
  OcclusionType occ_type;
};

class Light
{
public:
  virtual LightSample sample_emission(const Intersection& isect, scalar r1, scalar r2) const = 0;
};

class EnvironmentLight
{
public:
  EnvironmentLight(Texture2D* tex, bool spherical=true);

  EnvironmentLight(const EnvironmentLight& rhs) = delete;

  spectrum emission(const Vec3& dir) const;

private:
  bool spherical_coords;
  Texture2D* texture;
};

class DirectionalLight : public Light
{
public:
  // direction: direction of
  DirectionalLight(const Vec3& dir_source, const spectrum& r) : direction(dir_source), emission(r)
  {
  }

  LightSample sample_emission(const Intersection& isect, scalar r1, scalar r2) const override;

private:
  Vec3 direction; // source of the light
  spectrum emission;
};

class PointLight : public Light
{
public:
  PointLight(const Vec3& pos, const spectrum& r) : position(pos), emission(r)
  {
  }

  LightSample sample_emission(const Intersection& isect, scalar r1, scalar r2) const override;

private:
  Vec3 position;
  spectrum emission;
};
