#pragma once

#include "geometry.h"
#include "model.h"

class MeshTri;

class Mesh : public Primitive
{
public:
  Mesh(const RawModel& model);

  scalar intersect(const Ray& r, const Geometry*& geom) const override;

  const Vec3& pos(int i) const
  {
    return verts[i].position;
  }

  const bool is_differential;

private:
  friend class MeshTri;
  vector<Vertex> verts;
  vector<MeshTri> tris;
};

class MeshTri : public Geometry
{
public:
  MeshTri(const Mesh* m, const int v[3]);
  scalar intersect(const Ray& r) const;

  Vec3 normal(const Vec3& point) const override;
  Vec3 sample_shadow_ray_dir(const Intersection& isect,
                             scalar r1, scalar r2) const override;

  bool is_differential() const override
  {
    return mesh->is_differential;
  }

  void texture_coord(const Vec3& pos, const Vec3& normal,
                     scalar& u, scalar& v, Vec3& dpdu, Vec3& dpdv) const override;

  virtual ~MeshTri() {}

private:
  const Vertex& _v(int i) { return mesh->verts[vi[i]]; }
  const Vec3& _p(int i) const { return mesh->pos(vi[i]); }

  const Mesh* mesh;
  int vi[3];
  Vec3 e1, e2, n;
};
