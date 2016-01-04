#pragma once

#include "integrator.h"
#include <vector>

using std::vector;

#ifdef FEATURE_BIDIRECTIONAL

struct BDPTOptions
{
  size_t light_path_max_length;
  size_t eye_path_max_length;
  scalar power_heuristic_power;

  // min path length before russian roulette starts.
  size_t rr_min_length;

  // probability of terminating at a given spot for russian roulette
  scalar rr_terminate_p;
};

enum PVTag
{
  PV_INTERSECTION = 0,
  PV_EYE,
  PV_LIGHT,
  PV_ENV
};

class BidirectionalPathTracer : public RayIntegrator
{
public:
  struct EyeVertex
  {
    EyeVertex(const Ray& r, scalar p) : ray(r), prob(p) { }

    Ray ray;
    scalar prob;
  };

  struct PathVertex
  {
    PathVertex(const IntersectionView& view, const spectrum& refl, scalar p)
      : isect_view(view), next_refl(refl), prob(p) { }

    IntersectionView isect_view; // intersection information for this vertex

    spectrum next_refl; // reflectance between the previous and next vertex
                        // locations, not including the N.L components.

    scalar prob; // probability that this vertex was chosen, given the previous
                 // path up to this point
  };

  struct LightVertex
  {
    LightVertex() { }
    LightVertex(const EmissionSample& es)
      : emission_sample(es)
    {
    }

    EmissionSample emission_sample;
  };

  struct Path
  {
    vector<PathVertex> path;

    void add_vertex(const IntersectionView& isect, const spectrum& refl, scalar prob)
    {
      path.emplace_back(isect, refl, prob);
    }
  };

  struct LightPath : public Path
  {
    LightPath() { }

    LightVertex light;

    void add_light(const EmissionSample& es)
    {
      light.emission_sample = es;
    }
  };

  struct EyePath : public Path
  {
    EyePath(const Ray& r) : eye(r, 1.0)
    {
    }

    EyeVertex eye;


    bool trivial() const
    {
      return path.empty();
    }

    spectrum trivial_emission(const Scene&) const;
  };

  BidirectionalPathTracer(const BDPTOptions&);

  spectrum trace_ray(const Scene& scene, const Ray& ray, Sampler&, Sampler&) const;

  virtual ~BidirectionalPathTracer() { }

private:
  spectrum sample_pixel(Sampler& sampler);

  /* Create a path from a random light. */
  LightPath construct_light_path(const Scene& scene, Sampler& sampler) const;

  /* Create a path starting from the camera at a particular point. */
  EyePath construct_eye_path(const Ray& eye_ray, const Scene& scene,
                             Sampler& sampler) const;

  /* Create a path, given a set of starting conditions */
  void construct_path(const Scene& scene, const Ray& start, scalar init_p,
                      size_t max_length, Sampler& sampler, Path&) const;

  BDPTOptions opt;
};

#endif
