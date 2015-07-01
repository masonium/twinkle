#pragma once

#include "integrator.h"

struct BDPTOptions
{
  int light_path_max_length;
  int eye_path_max_length;
  scalar power_heuristic_power;
};

enum PVTag
{
  PV_INTERSECTION = 0,
  PV_EYE,
  PV_ENV
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
    PathVertex(Vec3 v, PVTag pvt) : vec(v), pv_tag(pvt) {}

    Vec3 vec;
    PVTag pv_tag;
  };

  BidirectionalPathTracer(const BDPTOptions&);

  void render(const Camera* cam, const Scene* scene, Film& film);

  virtual ~BidirectionalPathTracer() { }

private:
  spectrum sample_pixel(Sampler& sampler);

  void construct_light_path(Sampler& sampler, const Scene* scene, vector<PathVertex>& verts);
  void construct_eye_path(const Film& film, int x, int y,
                          Sampler& sampler, const Camera* cam,
                          const Scene* scene, vector<PathVertex>& verts);

  BDPTOptions opt;
};
