#pragma once

#include <string>
#include <vector>
#include "geometry.h"
#include "vec3.h"
#include "vec2.h"

#include "bounds.h"

using std::string;
using std::vector;

namespace RawModelLoad
{
  struct tri_ref;
  struct vertex_ref;
}

struct Vertex
{
  Vertex() = default;

  Vec3 position;
  Vec3 normal;
  Vec3 dpdu, dpdv;
  Vec2 uv;
};

struct Triangle
{
  uint v[3];
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

  RawModelLoadStatus load_raw_model(const vector<Vertex>& verts,
                                    const vector<Triangle>& tris,
                                    bool has_normal, bool has_tex);

  enum class FileFormat : uint8_t
  {
    GUESS = 0,
      STL = 1,
      OBJ = 2
      };

  /**
   * Load, possibly intellgently, based on the file suffix.
   */
  RawModelLoadStatus load(string filename, FileFormat format = FileFormat::GUESS);

  /**
   * Load Wavefront obj files.
   */
  RawModelLoadStatus load_from_obj(string filename);

  /**
   * Load .stl files.
   */
  RawModelLoadStatus load_from_stl(string filename);

  RawModel merge_vertices(scalar eps = EPSILON) const;

  /** 
   * Rescale (and translate) the mode to fit inside the specified bounding object.
   */
  void rescale(const bounds::AABB& new_bb, bool preserve_aspect_ratio = true);
  void rescale(const bounds::Sphere& new_bs);

  /***
   * center the model on the origin
   */
  void translate_to_origin();

  /*
   * Flip the order of the triangles.
   */
  void flip_tris();

  bounds::AABB bounding_box( ) const;
  bounds::Sphere bounding_sphere( ) const;

  const vector<Vertex>& verts() const & { return verts_; }
  vector<Vertex>&& verts() && { return std::move(verts_); }
  const vector<Triangle>& tris() const & { return tris_; }
  vector<Triangle>&& tris() && { return std::move(tris_); }

  bool has_tex() const { return has_tex_; }

private:
  vector<Vertex> verts_;
  vector<Triangle> tris_;

  bool has_tex_;

  // equal-weighted normals
  void compute_normals();

  // normalize
  void clear();

  RawModelLoadStatus load_from_parts(const vector<Vec3>& vertex_list, const vector<Vec3>& normal_list,
                                     const vector<Vec2>& tc_list,
                                     const vector<RawModelLoad::vertex_ref>& vertex_ref_list,
                                     const vector<RawModelLoad::tri_ref>& tri_list);
};
