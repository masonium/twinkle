#include <algorithm>
#include "kdmesh.h"
#include "mesh.h"

using std::copy;

MeshTri::MeshTri(const Mesh* m, const int v[3]) :
  mesh(m),
  e1(mesh->pos(v[1]) - mesh->pos(v[0])),
  e2(mesh->pos(v[2]) - mesh->pos(v[0]))
{
  Vec3 N = e1.cross(e2);
  inv_a_double = 1.0 / N.norm();
  n = N * inv_a_double;
  copy(v, v+3, vi);
}

scalar MeshTri::intersect(const Ray& ray, scalar max_t, SubGeo& geo) const
{
  return ray_triangle_intersection(ray, _p(0), _p(1), _p(2), max_t);
}

Vec3 MeshTri::normal(SubGeo geo, const Vec3& point) const
{
  return n;
}

bounds::AABB MeshTri::get_bounding_box() const
{
  return bounds::AABB(min(min(_p(0), _p(1)), _p(2)),
                      max(max(_p(0), _p(1)), _p(2)));
}


void MeshTri::texture_coord(SubGeo subgeo, const Vec3& pos, const Vec3& normal,
                   scalar& u, scalar& v) const
{
  const Vec3 P = pos - _p(0);
  const Vec3 n1 = e1.cross(P), n2 = P.cross(e2);
  scalar bu  = n1.dot(n) * inv_a_double, bv = n2.dot(n) * inv_a_double;
  Vec3 bary{1 - (bu + bv), bv, bu};

  u = mesh->uv(vi[0])[0] * bary.v[0] + mesh->uv(vi[1])[0] * bary.v[1] + mesh->uv(vi[2])[0] * bary.v[2];
  v = mesh->uv(vi[0])[1] * bary.v[0] + mesh->uv(vi[1])[1] * bary.v[1] + mesh->uv(vi[2])[1] * bary.v[2];
}


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
  MeshTri const* tri = nullptr;
  SubGeo dummy;
  scalar t = kd_tree->intersect(r, max_t, tri, dummy);
  if (t > 0)
    geo = tri - &tris[0];
  return t;
}
