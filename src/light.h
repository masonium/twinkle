#pragma once

#include "base.h"
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
  LightSample(spectrum s, scalar p, const Vec3& p1, const Vec3& p2 );
  LightSample(spectrum s, scalar p, const Ray& r);


  operator bool() const
  {
    return occ_type != OCCLUSION_NONE;
  }

  Vec3 direction( ) const { return ray.direction.normal(); }
  scalar p() const { return p_; }

  bool is_occluded( const Scene& scene ) const;

  spectrum emission() const { return em; }

private:
  Ray ray;
  spectrum em;
  scalar p_;
  OcclusionType occ_type;
};

class Light : public Base
{
public:
  virtual LightSample sample_emission(const Intersection& isect, Sampler&) const = 0;

  virtual std::string to_string() const { return "Light"; }

  virtual ~Light() { }
};

class DirectionalLight : public Light
{
public:
  // direction: direction of
  DirectionalLight(const Vec3& dir_source, const spectrum& r) : direction(dir_source.normal()), emission(r)
  {
  }

  LightSample sample_emission(const Intersection& isect, Sampler&) const override;

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

  LightSample sample_emission(const Intersection& isect, Sampler&) const override;

  std::string to_string() const override;

private:
  Vec3 position;
  spectrum emission;
};
