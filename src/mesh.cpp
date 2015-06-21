#include <algorithm>
#include "kdmesh.h"
#include "mesh.h"

using std::copy;

MeshTri::MeshTri(const Mesh* m, const int v[3]) :
  mesh(m),
  e1(mesh->pos(v[1]) - mesh->pos(v[0])),
  e2(mesh->pos(v[2]) - mesh->pos(v[0])),
  n(e1.cross(e2))
{
  copy(v, v+3, vi);
}

scalar MeshTri::intersect(const Ray& ray, scalar max_t) const
{
  return ray_triangle_intersection(ray, _p(0), _p(1), _p(2), max_t);
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

bounds::AABB MeshTri::get_bounding_box() const
{
  return bounds::AABB(min(min(_p(0), _p(1)), _p(2)),
                      max(max(_p(0), _p(1)), _p(2)));
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

scalar Mesh::intersect(const Ray& r, scalar max_t, const Geometry*& geom) const
{
  scalar best_t = numeric_limits<scalar>::max();
  geom = nullptr;
  for (const auto& tri: tris)
  {
    scalar t = tri.intersect(r, best_t);
    if (t > 0)
    {
      best_t = t;
      geom = &tri;
    }
  }
  return geom == nullptr ? -1 : best_t;
}

////////////////////////////////////////////////////////////////////////////////

KDMesh::KDMesh(const RawModel& model) : Mesh(model)
{
  auto tri_addresses = vector<const MeshTri*>{tris.size()};
  for (auto i = 0u; i < tris.size(); ++i)
    tri_addresses[i] = &tris[i];

  kd::TreeOptions opt;
  kd_tree = std::make_shared<kd::Tree<MeshTri const*>>(tri_addresses, opt);
}

scalar KDMesh::intersect(const Ray& r, scalar max_t, const Geometry*& geom) const
{
  return -1;
}
