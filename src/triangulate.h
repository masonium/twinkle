#pragma once

#include "vec3.h"
#include "geometry/implicit.h"
#include "model.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <iostream>

using std::ostream;
using std::vector;
using std::map;

using EdgeVertMap = std::unordered_map<size_t, size_t>;

/*
  Computes the marching tetrahedra isosurface for an implicit function.
 */
class MarchingTetrahedra
{
public:
  MarchingTetrahedra(ImplicitEvalFunc f, const bounds::AABB& box, size_t div);

  /**
   * Return a model of the surface.
   */
  RawModel model() const;

  const auto& tris() const { return _tris; }
private:

  // Fill in the vector containing all of the grid-sampled points for the
  // implicit surface.
  void precompute_grid_vertices(ImplicitEvalFunc f);

  // Compute all of the triangles for a given cube.
  void compute_cube(size_t cx, size_t cy, size_t cz);

  void add_one_triangle_case(size_t cube_idx,
                             uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3);
  void add_two_triangles_case(size_t cube_idx,
                              uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3);
  void add_tri(size_t e0, size_t e1, size_t e2);

  // Build the list of vertices and a map from edge vertices to contiguous
  // vertices.
  void build_vertex_and_map(vector<Vertex>&, EdgeVertMap&) const;

  // Remap the triangle indices using the provided edge map, returning a new set
  // of triangles.
  vector<Triangle> remap_triangles(const EdgeVertMap&) const;

  // Return a geometric vertex associated with a global edge index.
  Vec3 get_edge_vertex_position(size_t e) const;

  Vec3 get_vertex_position(size_t cube_index) const;

  // Return the global index of the edge, given the local vertices and the cube.
  size_t global_edge_index(size_t cube_index, uint8_t lv0, uint8_t lv1) const;

  // Return the global index of the vertex, given the cube index and local
  // vertex
  size_t global_vertex_index(size_t cube_index, uint8_t v) const;


  bounds::AABB _bbox;

  // points per size (one more than the number of cubes per side)
  size_t _pps;
  scalar _inv_div;

  // sampled at each grid point
  vector<scalar> _field_samples;

  // triangles in the resulting model
  vector<Triangle> _tris;

  size_t _lookup_offsets[8];

};
