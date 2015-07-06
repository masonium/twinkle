#pragma once

#include "geometry.h"
#include "model.h"
#include "bounds.h"
#include "kdtree.h"

class MeshTri;

class Mesh : public Geometry
{
public:
  Mesh(const RawModel& model);

  virtual scalar intersect(const Ray& r, scalar max_t, SubGeo& geom) const override;

  Vec3 normal(SubGeo, const Vec3& point) const override;

  bool is_differential() const override
  {
    return is_diff;
  }

  void texture_coord(SubGeo, const Vec3& pos, const Vec3& normal,
                     scalar& u, scalar& v) const override;

  const Vec3& pos(int i) const
  {
    return verts[i].position;
  }
  const Vec3& n(int i) const
  {
    return verts[i].normal;
  }


  virtual ~Mesh() { }

protected:
  friend class MeshTri;
  const bool is_diff;

  vector<Vertex> verts;
  vector<MeshTri> tris;
};

class MeshTri : public SimpleGeometry, public Bounded
{
public:
  MeshTri(const Mesh* m, const int v[3]);

  scalar intersect(const Ray& r, scalar max_t, SubGeo& geo) const override;

  Vec3 normal(SubGeo, const Vec3& point) const override;

  // void texture_coord(SubGeo, const Vec3& pos, const Vec3& normal,
  //                    scalar& u, scalar& v) const override;

  bounds::AABB get_bounding_box() const;

  virtual ~MeshTri() {}

private:
  const Vertex& _v(int i) const { return mesh->verts[vi[i]]; }
  const Vec3& _p(int i) const { return mesh->pos(vi[i]); }

  const Mesh* mesh;
  int vi[3];
  Vec3 e1, e2, n;
};
