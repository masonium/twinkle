#include <algorithm>
#include "mesh.h"
#include "geometry/isect_util.h"

using std::copy;

MeshTri::MeshTri(const Mesh* m, const int v[3]) :
  mesh(m)
{
  copy(v, v+3, vi);
}

scalar_fp MeshTri::intersect(const Ray& ray, scalar_fp max_t, SubGeo& geo) const
{
  return ray_triangle_intersection(ray, _p(0), _p(1), _p(2), max_t);
}

Vec3 MeshTri::normal(const Vec3& point) const
{
  return (_p(1) - _p(0)).cross(_p(2) - _p(0)).normal();
}

bounds::AABB MeshTri::get_bounding_box() const
{
  return bounds::AABB(min(min(_p(0), _p(1)), _p(2)),
                      max(max(_p(0), _p(1)), _p(2)));
}


void MeshTri::texture_coord(const Vec3& pos, const Vec3& normal,
                            scalar& u, scalar& v) const
{
  Vec3 e1(_p(1) - _p(0));
  Vec3 e2(_p(2) - _p(0));
  Vec3 N = e1.cross(e2);
  scalar inv_a_double = 1.0 / N.norm();
  Vec3 n = N * inv_a_double;

  const Vec3 P = pos - _p(0);
  const Vec3 n1 = e1.cross(P), n2 = P.cross(e2);
  scalar bu  = n1.dot(n) * inv_a_double, bv = n2.dot(n) * inv_a_double;
  Vec3 bary{1 - (bu + bv), bv, bu};

  u = mesh->uv(vi[0])[0] * bary.v[0] + mesh->uv(vi[1])[0] * bary.v[1] + mesh->uv(vi[2])[0] * bary.v[2];
  v = mesh->uv(vi[0])[1] * bary.v[0] + mesh->uv(vi[1])[1] * bary.v[1] + mesh->uv(vi[2])[1] * bary.v[2];
}


////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(const RawModel& model) : is_diff(model.has_tex())
{
  verts = model.verts();
  for (const auto& a: model.tris())
  {
    tris.emplace_back(this, a.v);
  }
}

Mesh::Mesh(RawModel&& model) : is_diff(model.has_tex())
{
  verts = model.verts();
  for (const auto& a: model.tris())
  {
    tris.emplace_back(this, a.v);
  }
}

bounds::AABB Mesh::get_bounding_box() const
{
  auto bb = bounds::AABB{Vec3::zero, Vec3::zero};
  return accumulate(tris.begin(), tris.end(), bb,
                    [](const auto& bb, const auto& tri)
                    {
                      return bounds::AABB::box_union(bb, tri.get_bounding_box());
                    });
}

scalar_fp Mesh::intersect(const Ray& r, scalar_fp max_t, SubGeo& subgeo) const
{
  scalar_fp best_t = max_t;
  for (auto i = 0u; i < tris.size(); ++i)
  {
    auto& tri = tris[i];
    auto t = tri.intersect(r, best_t, subgeo);
    if (t < best_t)
    {
      best_t = t;
      subgeo = i;
    }
  }
  return best_t;
}

Vec3 Mesh::normal(SubGeo subgeo, const Vec3& point) const
{
  return tris[subgeo].normal(point);
}

void Mesh::texture_coord(SubGeo subgeo, const Vec3& pos, const Vec3& normal,
                         scalar& u, scalar& v) const
{
  return tris[subgeo].texture_coord(pos, normal, u, v);
}


