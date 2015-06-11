#include "mesh.h"
#include <algorithm>

using std::copy;

MeshTri::MeshTri(const Mesh* m, const int v[3]) :
  mesh(m)
{
  copy(v, v+3, vi);
  e1 = mesh->pos(vi[1]) - mesh->pos(vi[0]);
  e2 = mesh->pos(vi[2]) - mesh->pos(vi[0]);
  n = e1.cross(e2);
}

scalar MeshTri::intersect(const Ray& ray) const
{
  return ray_triangle_intersection(_p(0), _p(1), _p(2), ray);
}

Vec3 MeshTri::normal(const Vec3& point) const
{
  return n.normal();
}

Vec3 MeshTri::sample_shadow_ray_dir(const Intersection& isect,
                                    scalar r1, scalar r2) const
{
  scalar sum = r1 + r2;
  if (sum > 1.0)
  {
    scalar excess = sum - 1.0;
    r1 -= excess;
    r2 -= excess;
  }

  scalar r3 = 1 - r1 - r2;
  return r1 * mesh->pos(vi[0]) + r2 * mesh->pos(vi[1]) + r3 * mesh->pos(vi[2]);
}
void MeshTri::texture_coord(const Vec3& pos, const Vec3& normal,
                            scalar& u, scalar& v, Vec3& dpdu, Vec3& dpdv) const
{
}

////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(const RawModel& model) : is_differential(model.has_tex)
{
  verts.resize(model.verts.size());
  copy(model.verts.begin(), model.verts.end(), verts.begin());
  for (const auto& a: model.tris)
  {
    tris.emplace_back(this, a.v);
  }
}

scalar Mesh::intersect(const Ray& r, const Geometry*& geom) const
{
  scalar best_t = -1;
  geom = nullptr;
  for (const auto& tri: tris)
  {
    scalar t = tri.intersect(r);
    if (t > 0)
    {
      if (best_t < 0 || t < best_t)
      {
        best_t = t;
        geom = &tri;
      }
    }
  }
  return best_t;
}
