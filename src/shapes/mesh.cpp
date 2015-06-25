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

scalar MeshTri::intersect(const Ray& ray, scalar max_t, SubGeo& geo) const
{
  return ray_triangle_intersection(ray, _p(0), _p(1), _p(2), max_t);
}

Vec3 MeshTri::normal(SubGeo geo, const Vec3& point) const
{
  return n.normal();
}

bounds::AABB MeshTri::get_bounding_box() const
{
  return bounds::AABB(min(min(_p(0), _p(1)), _p(2)),
                      max(max(_p(0), _p(1)), _p(2)));
}

/*
void MeshTri::texture_coord(const Vec3& pos, const Vec3& normal,
                            scalar& u, scalar& v) const
{
}
*/

////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(const RawModel& model) : is_diff(model.has_tex)
{
  verts.resize(model.verts.size());
  copy(model.verts.begin(), model.verts.end(), verts.begin());
  for (const auto& a: model.tris)
  {
    tris.emplace_back(this, a.v);
  }
}

scalar Mesh::intersect(const Ray& r, scalar max_t, SubGeo& subgeo) const
{
  scalar best_t = numeric_limits<scalar>::max();
  bool found_isect = false;
  for (auto i = 0u; i < tris.size(); ++i)
  {
    auto& tri = tris[i];
    scalar t = tri.intersect(r, best_t, subgeo);
    if (t > 0)
    {
      found_isect = true;
      best_t = t;
      subgeo = i;
    }
  }
  return found_isect ? best_t : -1;
}

Vec3 Mesh::normal(SubGeo subgeo, const Vec3& point) const
{
  return tris[subgeo].normal(subgeo, point);
}

void Mesh::texture_coord(SubGeo subgeo, const Vec3& pos, const Vec3& normal,
                         scalar& u, scalar& v) const
{
  return tris[subgeo].texture_coord(subgeo, pos, normal, u, v);
}


////////////////////////////////////////////////////////////////////////////////

KDMesh::KDMesh(const RawModel& model) : Mesh(model)
{
  auto tri_addresses = vector<const MeshTri*>{tris.size()};
  for (auto i = 0u; i < tris.size(); ++i)
    tri_addresses[i] = &tris[i];

  kd::TreeOptions opt;
  opt.max_elements_per_leaf = 10;
  kd_tree = std::make_shared<tri_tree>(tri_addresses, opt);
}

scalar KDMesh::intersect(const Ray& r, scalar max_t, SubGeo& geo) const
{
  MeshTri const* tri;
  SubGeo dummy;
  scalar t = kd_tree->intersect(r, max_t, tri, dummy);
  if (t > 0)
    geo = tri - &tris[0];
  return t;
}
