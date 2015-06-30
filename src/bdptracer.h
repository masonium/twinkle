#pragma once

#include "integrator.h"

struct BDPTOptions
{
  int light_path_max_length;
  int eye_path_max_length;
  scalar power_heuristic_power;
};



class BidirectionalPathTracer : public Integrator
{
public:
  struct EnvironmentLightVertex
  {
    Vec3 direction;
  };

  struct PathVertex
  {
    union
    {
      Intersection isect;
    };
  };

  BidirectionalPathTracer(const BDPTOptions&);

  void render(const Camera* cam, const Scene* scene, Film& film);

  virtual ~BidirectionalPathTracer() { }

private:
  spectrum sample_pixel(Sampler& sampler);

  void construct_light_path(Sampler& sampler, vector<PathVertex>& verts);
  void construct_eye_path(Sampler& sampler, const Camera* cam,
                          const Scene* scene,vector<PathVertex>& verts);

  BDPTOptions opt;
};
