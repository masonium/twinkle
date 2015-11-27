#pragma once

#include "geometry.h"
#include "model.h"
#include "bounds.h"

class MeshTri;

/**
 * Acceleration structure for faster triangle intersections.
 *
 * The intersection routine uses barycentric coordinates, projected onto a 2D
 * axis-aligned plane for faster computations. The plane chosen is the one with
 * the largest projected area of the triangle (i.e. the largets normal
 * component, by magnitude).
 *
 * The acceleration structure stores many of the required intermediate
 * values. It also takes up less than a cache line of space, and requires only
 *
 */
struct MeshTriAccel
{
  MeshTriAccel(const Vec3& p0, const Vec3& p1, const Vec3& p2);

  scalar nu, nv;
  scalar nd;
  int32_t k;
  scalar b_nu, b_nv;
  scalar b_d;
  int32_t pad;
  scalar c_nu;
  scalar c_nv;
  scalar c_d;
  int32_t pad2;
};

MeshTriAccel compute_meshtri_accel(const Vec3& p0, const Vec3& p1, const Vec3& p2);

class Mesh : public Geometry
{
public:
  template <typename T>
  Mesh(T&& model) : is_diff(model.has_tex())
  {
    verts = model.verts();
    auto i = 0u;
    for (const auto& a: model.tris())
    {
      tris.emplace_back(this, i++, a.v);
      accels.emplace_back(verts[a.v[0]].position, verts[a.v[1]].position, verts[a.v[2]].position);
    }
  }
  virtual scalar_fp intersect(const Ray& r, scalar_fp max_t, SubGeo& geom) const override;

  Vec3 normal(SubGeo, const Vec3& point) const override;

  bool is_differential() const override
  {
    return is_diff;
  }

  void texture_coord(SubGeo, const Vec3& pos, const Vec3& normal,
                     scalar& u, scalar& v) const override;

  bool is_bounded() const override
  {
    return true;
  }

  bounds::AABB get_bounding_box() const;

  const MeshTriAccel& accel(int i) const
  {
    return accels[i];
  }

  const Vec3& pos(int i) const
  {
    return verts[i].position;
  }
  const Vec3& n(int i) const
  {
    return verts[i].normal;
  }

  const Vec2& uv(int i) const
  {
    return verts[i].uv;
  }

  virtual ~Mesh() { }

protected:
  friend class MeshTri;
  const bool is_diff;

  void _precompute_accel();

  vector<Vertex> verts;
  vector<MeshTri> tris;
  vector<MeshTriAccel> accels;
};

class MeshTri
{
public:
  MeshTri(const Mesh* m, int f, const int v[3]);

  scalar_fp intersect(const Ray& r, scalar_fp max_t, SubGeo&) const ;

  Vec3 normal(const Vec3& point) const ;

  virtual void texture_coord(const Vec3& pos, const Vec3& normal,
                             scalar& u, scalar& v) const ;

  bool is_bounded() const
  {
    return true;
  }

  virtual bounds::AABB get_bounding_box() const;

  virtual ~MeshTri() {}

private:
  friend class Mesh;
  const Vertex& _v(int i) const { return mesh->verts[vi[i]]; }
  const Vec3& _p(int i) const { return mesh->pos(vi[i]); }

  const Mesh* mesh;
  int vi[3];
  int ti;
};
