#pragma once

#include "geometry.h"
#include "vec3.h"
#include <string>
#include <vector>
#include "bounds.h"

using std::string;
using std::vector;

namespace RawModelLoad
{
  struct tex_coord;
  struct tri_ref;
  struct vertex_ref;
}

struct Vertex
{
  Vec3 position;
  Vec3 normal;
  Vec3 dpdu, dpdv;
  scalar u, v;
};

struct Triangle
{
  int v[3];
};

struct RawModelLoadStatus
{
  bool success;
  bool has_normals;
  bool has_tex;
};

class RawModel
{
public:
  RawModel();

  RawModelLoadStatus load_obj_model(string filename);
  RawModelLoadStatus load_stl_model(string filename);

  vector<Vertex> verts;
  vector<Triangle> tris;

  bool has_tex;

  RawModel merge_vertices(scalar eps = EPSILON) const;
  
  void rescale(const bounds::AABB& new_bb);
  void rescale(const bounds::Sphere& new_bs);

  bounds::AABB bounding_box( ) const;
  bounds::Sphere bounding_sphere( ) const;


private:
  // equal-weighted normals
  void compute_normals();

  // normalize
  void clear();

  RawModelLoadStatus load_from_parts(const vector<Vec3>& vertex_list, const vector<Vec3>& normal_list,
                                     const vector<RawModelLoad::tex_coord>& tc_list,
                                     const vector<RawModelLoad::vertex_ref>& vertex_ref_list,
                                     const vector<RawModelLoad::tri_ref>& tri_list);
};
