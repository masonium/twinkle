#pragma once

#include "geometry.h"
#include "model.h"
#include "bounds.h"

class MeshTri;

class Mesh : public Geometry
{
public:
  Mesh(const RawModel& model);
  Mesh(RawModel&& model);

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

  vector<Vertex> verts;
  vector<MeshTri> tris;
};

class MeshTri
{
public:
  MeshTri(const Mesh* m, const int v[3]);

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
  const Vertex& _v(int i) const { return mesh->verts[vi[i]]; }
  const Vec3& _p(int i) const { return mesh->pos(vi[i]); }

  const Mesh* mesh;
  int vi[3];
};