#include "bdptracer.h"

BidirectionalPathTracer::BidirectionalPathTracer(const BDPTOptions& opt_)
  : opt(opt_)
{
}

void BidirectionalPathTracer::render(const Camera* cam, const Scene* scene, Film& film)
{

}

void BidirectionalPathTracer::construct_light_path(Sampler& sampler, const Scene* scene,
                                                   vector<PathVertex>& verts)
{

}

void BidirectionalPathTracer::construct_eye_path(
  const Film& film, int x, int y,
  Sampler& sampler, const Camera* cam,
  const Scene* scene, vector<PathVertex>& verts)
{
  auto ps = cam->sample_pixel(film.width, film.height, x, y, sampler);
  verts.emplace_back(ps.ray.position, PV_EYE);

  auto ray = ps.ray;
  for (auto i = 0; i < opt.eye_path_max_length; ++i)
  {
    auto isect = scene->intersect(ray);
    if (!isect)
    {
      verts.emplace_back(ray.direction.normal(), PV_ENV);
      break;
    }

    //verts.emplace_back(isect);
    if (isect.is_emissive())
      break;

    scalar brdf_p, refl;
    Vec3 new_ray_dir = isect.sample_bsdf(-ray.direction.normal(), sampler, brdf_p, refl);
    if (brdf_p == 0)
      break;

    ray = Ray(isect.position, new_ray_dir).nudge();
  }
}
