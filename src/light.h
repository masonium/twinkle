#pragma once

#include "base.h"
#include "spectrum.h"
#include "texture.h"
#include "shape.h"

class Scene;
class Light;

enum OcclusionType
{
  OCCLUSION_RAY,
  OCCLUSION_POINTS,
  OCCLUSION_NONE
};

/*
 * Sampled emission from a specific light towards a specific intersection point.
 */
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

enum class EmissionType
{
  EMISSION_POINT,
  EMISSION_RAY,
  EMISSION_NONE
};

/*
 * Sampled emission to a scene.
 */
class EmissionSample
{
public:
  EmissionSample() : light(nullptr), ray(Vec3::zero, Vec3::z_axis) { }
  EmissionSample(Light const* l, const Ray& r)  : light(l), ray(r) {}

  Light const* light;
  Ray ray;

  // probability of this specific light being chosen
  scalar light_prob;

  // Probability of the ray being chosen, conditioned on the specific light.
  scalar ray_prob;
};

class Light : public Base
{
public:
  /* Given an intersection point, sample an emission from the point. */
  virtual LightSample sample_emission(const Intersection& isect, Sampler&) const = 0;

  /* Sample an emission generically, from anywhere within the scene. */
  virtual EmissionSample sample_emission(const Scene& scene, Sampler&) const = 0;

  /* Compute the emission from the light to the the intersection point, given
     the chosen emission sample.
   */
  virtual  spectrum emission(const Scene& scene, const IntersectionView& isect,
                             const EmissionSample& sample) const = 0;

  virtual std::string to_string() const { return "Light"; }

  virtual ~Light() { }
};

class DirectionalLight : public Light
{
public:
  // direction: direction of the (distant) light source
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
