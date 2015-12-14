#include <algorithm>
#include <iterator>
#include <iostream>
#include "mesh.h"
#include "geometry/isect_util.h"

using std::copy;
using std::ostream_iterator;
using std::cerr;

MeshTri::MeshTri(const Mesh* m, int f, const uint v[3]) :
  mesh(m), ti(f)
{
  copy(v, v+3, vi);
}

scalar_fp ray_triangle_intersection_accel(const Ray& ray, scalar_fp max_t, const MeshTriAccel& accel)
{
  auto u = (accel.k + 1) % 3, v = (accel.k + 2) % 3;
  auto denom = (ray.direction[u] * accel.nu + ray.direction[v] * accel.nv + ray.direction[accel.k]);
  if (fabs(denom) < 0.0001)
    return sfp_none;
  auto num = accel.nd - ray.position[u] * accel.nu - ray.position[v] * accel.nv - ray.position[accel.k];
  
  scalar t = num / denom;
  if (t < EPSILON)
    return sfp_none;

  scalar_fp t_prop(t);
  if (max_t < t_prop)
    return sfp_none;

  scalar hu = ray.position[u] + ray.direction[u] * t;
  scalar hv = ray.position[v] + ray.direction[v] * t;

  scalar beta = hu * accel.b_nu + hv * accel.b_nv + accel.b_d;
  if (beta < 0)
    return sfp_none;

  scalar gamma = hu * accel.c_nu + hv * accel.c_nv + accel.c_d;
  if (gamma < 0)
    return sfp_none;

  scalar alpha = 1 - beta - gamma;
  if (alpha < 0 || alpha > 1)
    return sfp_none;

  return t_prop;
}

scalar_fp MeshTri::intersect(const Ray& ray, scalar_fp max_t, SubGeo& geo) const
{
  return ray_triangle_intersection_accel(ray, max_t, mesh->accel(ti));
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
                            scalar& uv_u, scalar& uv_v) const
{
  const auto& accel = mesh->accel(ti);

  auto u = (accel.k + 1) % 3, v = (accel.k + 2) % 3;
  scalar hu = pos[u];
  scalar hv = pos[v];

  scalar beta = hu * accel.b_nu + hv * accel.b_nv + accel.b_d;
  scalar gamma = hu * accel.c_nu + hv * accel.c_nv + accel.c_d;
  scalar alpha = 1 - beta - gamma;

  auto uv = mesh->uv(vi[0]) * alpha + mesh->uv(vi[1]) * gamma + mesh->uv(vi[2]) * beta;
  uv_u = uv[0];
  uv_v = uv[1];
}


////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

MeshTriAccel::MeshTriAccel(const Vec3& p0, const Vec3& p1, const Vec3& p2)
{
  const auto b = p1 - p0;
  const auto c = p2 - p0;

  const auto N = b.cross(c);
  const auto aN = N.abs();
  if (aN.x >= aN.y && aN.x >= aN.z)
    this->k = 0;
  else if (aN.y >= aN.x && aN.y >= aN.z)
    this->k = 1;
  else
    this->k = 2;
    
  int k = this->k;
  int u = (this->k + 1) % 3, v = (this->k + 2) % 3;
  this->nu = N[u] / N[k];
  this->nv = N[v] / N[k];

  this->nd = p0.dot(N) / N[k];

  scalar inv_area = 1.0 / (b[u] * c[v] - b[v] * c[u]);
  this->b_nu = -b[v] * inv_area;
  this->b_nv = b[u] * inv_area;
  this->b_d = (b[v] * p0[u] - b[u] * p0[v]) * inv_area;

  this->c_nu = c[v] * inv_area;
  this->c_nv = -c[u] * inv_area;
  this->c_d = (c[u] * p0[v] - c[v] * p0[u]) * inv_area;
}
