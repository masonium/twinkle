#include "bdptracer.h"

BidirectionalPathTracer::BidirectionalPathTracer(const BDPTOptions& opt_)
  : opt(opt_)
{
}

void BidirectionalPathTracer::render(const Camera* cam, const Scene* scene, Film& film)
{

}

void BidirectionalPathTracer::construct_eye_path(
  Sampler& sampler, const Camera* cam,
  const Scene* scene,vector<PathVertex>& verts)
{

}
