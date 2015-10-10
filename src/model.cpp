#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <limits>
#include "util.h"
#include "model.h"

using std::transform;
using std::ifstream;
using std::istringstream;
using std::vector;
using std::cerr;
using std::unordered_map;
using std::make_pair;
using std::pair;

namespace RawModelLoad
{
  struct tri_ref
  {
    int v[3];
  };

  struct vertex_ref
  {
    int p, t, n;
    bool operator==(const vertex_ref& b) const
    {
      return p == b.p && t == b.t && n == b.n;
    }
  };

}

RawModel::RawModel() : verts_(), tris_(), has_tex_(false)
{
}

namespace std
{
  template <>
  struct hash<RawModelLoad::vertex_ref>
  {
  public:
    size_t operator ()(const RawModelLoad::vertex_ref& vr) const
    {
      size_t seed = 0;
      hash_combine(seed, vr.n);
      hash_combine(seed, vr.p);
      hash_combine(seed, vr.t);
      return seed;
    }
  };
}
using namespace RawModelLoad;

vertex_ref parse_obj_vertex_ref(string token);

RawModelLoadStatus RawModel::load_raw_model(const vector<Vertex>& verts,
                                            const vector<Triangle>& tris, 
                                            bool has_normal, bool has_tex)
{
  this->verts_.clear();
  copy(verts.begin(), verts.end(), std::inserter(this->verts_, this->verts_.end()));

  this->tris_.clear();
  copy(tris.begin(), tris.end(), std::inserter(this->tris_, this->tris_.end()));

  if (!has_normal)
    compute_normals();

  this->has_tex_ = has_tex;
  return RawModelLoadStatus{.success=true, .has_normals=true, .has_tex=has_tex};
}

RawModelLoadStatus RawModel::load_obj_model(string filename)
{
  RawModelLoadStatus mls;
  mls.success = false;

  ifstream fin(filename);
  if (!fin)
  {
    cerr << "Could not load model.\n";
    return mls;
  }

  vector<Vec3> vertex_list;
  vector<Vec3> normal_list;
  vector<Vec2> tc_list;
  vector<vertex_ref> ref_list;
  vector<tri_ref> tri_list;

#define BUF_SIZE 1024
  char linebuf[BUF_SIZE];

  int line_count = 0;
  while (!fin.eof())
  {
    ++line_count;
    fin.getline(linebuf, BUF_SIZE);
    if (fin.fail())
    {
      if (fin.eof())
	break;

      cerr << "Line " << line_count << " in obj too long.";
      return mls;
    }
    string str(linebuf);

    if (str.empty() || str.find('#') != string::npos)
      continue;

    istringstream ss(str);

    string line_type;
    ss >> line_type;
    if (line_type == "v")
    {
      Vec3 v;
      ss >> v;
      vertex_list.push_back(v);
    }
    else if (line_type == "vn")
    {
      Vec3 v;
      ss >> v;
      normal_list.push_back(v);
    }
    else if (line_type == "vt")
    {
      Vec2 tc;
      ss >> tc.x >> tc.y;
      tc_list.push_back(tc);
    }
    else if (line_type == "f")
    {
      string part;
      int sidx = ref_list.size();
      int num_verts_ = 0;
      while (true)
      {
	ss >> part;
        if (!ss)
          break;
	ref_list.push_back(parse_obj_vertex_ref(part));
	++num_verts_;
      }
      for (int i = 0; i < num_verts_ - 2; ++i)
      {
	tri_ref tri{sidx, sidx+i+1, sidx+i+2};
	tri_list.emplace_back(tri);
      }
    }
    else if (line_type == "")
    {
      continue;
    }
    else
    {
      cerr << "unknown line type: " << line_type << "\n";
      return mls;
    }
  }

  mls = load_from_parts(vertex_list, normal_list,
			tc_list, ref_list,
			tri_list);

  if (mls.success && !mls.has_normals)
  {
    compute_normals();
    mls.has_normals = true;
  }
  has_tex_ = mls.has_tex;
  return mls;
}

RawModelLoadStatus RawModel::load_stl_model(string filename)
{
  RawModelLoadStatus mls;
  mls.success = false;

  ifstream fin(filename);
  if (!fin)
  {
    cerr << "Could not open model file (" << filename << ").\n";
    return mls;
  }

  vector<Vec3> vertex_list;
  vector<Vec3> normal_list;
  vector<Vec2> tc_list;
  vector<vertex_ref> ref_list;
  vector<tri_ref> tri_list;

#define BUF_SIZE 1024
  char linebuf[BUF_SIZE];

  int line_count = 0;
  while (!fin.eof())
  {
    ++line_count;
    fin.getline(linebuf, BUF_SIZE);
    if (fin.fail())
    {
      if (fin.eof())
	break;

      cerr << "Line " << line_count << " in obj too long.";
      return mls;
    }
    string str(linebuf);

    if (str.empty())
      continue;

    istringstream ss(str);

    string line_type;
    ss >> line_type;
    if (line_type == "vertex")
    {
      Vec3 v;
      ss >> v;
      vertex_list.push_back(v);
      ref_list.emplace_back(vertex_ref{static_cast<int>(ref_list.size()), -1, -1});
    }
    else if (line_type == "endloop")
    {
      int num_tris = tri_list.size();
      tri_ref tri{num_tris*3, num_tris*3+1, num_tris*3+2};
      tri_list.emplace_back(tri);
    }
    else if (line_type == "facet" || line_type == "outer" ||
             line_type == "endfacet" || line_type == "solid")
    {
    }
    else if (line_type == "endsolid")
      break;
    else
    {
      cerr << "unknown line type: " << line_type << "\n";
      return mls;
    }
  }

  mls = load_from_parts(vertex_list, normal_list,
			tc_list, ref_list,
			tri_list);

  if (mls.success && !mls.has_normals)
  {
    compute_normals();
    mls.has_normals = true;
  }
  has_tex_ = mls.has_tex;
  return mls;
}

RawModelLoadStatus RawModel::load_from_parts(const vector<Vec3>& vertex_list, const vector<Vec3>& normal_list,
					     const vector<Vec2>& tc_list, const vector<vertex_ref>& vertex_ref_list,
					     const vector<tri_ref>& tri_list)
{
  clear();

  unordered_map<vertex_ref, int> ref_map;

  RawModelLoadStatus mls;
  mls.success = true;
  mls.has_normals = normal_list.size() > 0;
  mls.has_tex = tc_list.size() > 0;

  for (const auto& tr: tri_list)
  {
    Triangle tri;
    for (int j = 0; j < 3; ++j)
    {
      int vidx = tr.v[j];
      auto vr = vertex_ref_list[vidx];

      // Add 'new' vertices.
      if (ref_map.find(vr) == ref_map.end())
      {
	ref_map.insert(make_pair(vr, ref_map.size()));
	Vertex v;

	v.position = vertex_list[vr.p];
	if (vr.t >= 0)
	{
	  v.uv = tc_list[vr.t];
	}
	if (vr.n >= 0)
	{
	  v.normal = normal_list[vr.n];
	}
	verts_.push_back(v);
      }

      tri.v[j] = ref_map[vr];
    }
    tris_.push_back(tri);
  }

  return mls;
}

void RawModel::clear()
{
  verts_.clear();
  tris_.clear();
  has_tex_ = false;
}

bounds::AABB RawModel::bounding_box() const
{
  Vec3 min{std::numeric_limits<scalar>::max()};
  Vec3 max{std::numeric_limits<scalar>::min()};
  min = accumulate(verts_.begin(), verts_.end(), min,
                   [] (const Vec3& v, const Vertex& w) { return ::min(v, w.position); });
  max = accumulate(verts_.begin(), verts_.end(), max,
                   [] (const Vec3& v, const Vertex& w) { return ::max(v, w.position); });

  return bounds::AABB{min, max};
}

bounds::Sphere RawModel::bounding_sphere() const
{
  Vec3 p1 = verts_[0].position;

  Vec3 p2;
  scalar max_dist2 = 0;
  for (const auto& v: verts_)
  {
    scalar dist2 = (v.position - p1).norm2();
    if (max_dist2 < dist2)
    {
      max_dist2 = dist2;
      p2 = v.position;
    }
  }

  Vec3 p3;
  max_dist2 = 0;
  for (const auto& v: verts_)
  {
    scalar dist2 = (v.position - p2).norm2();
    if (max_dist2 < dist2)
    {
      max_dist2 = dist2;
      p3 = v.position;
    }
  }

  // Take the midpoint as the center, and find the best radius
  Vec3 mid = (p2 + p3) * 0.5;

  scalar radius = sqrt(accumulate(verts_.begin(), verts_.end(), (mid - p2).norm2(),
                                  [&mid](scalar r2, const Vertex& v) {
                                    return std::max(r2, (mid - v.position).norm2());
                                  }));

  return bounds::Sphere(mid, radius);
}

void RawModel::flip_tris()
{
  for (auto& tri: tris_)
  {
    std::swap(tri.v[1], tri.v[2]);
  }

  // flipping the triangles inverts the normals
  for (auto& vert: verts_)
  {
    vert.normal = -vert.normal;
  }
}

void RawModel::compute_normals()
{
  vector<Vec3> normal_sum( verts_.size() );
  vector<scalar> normal_weight( verts_.size() );

  for (const auto& tri: tris_)
  {
    // compute the normal, and compute the angle
    auto p1 = verts_[tri.v[1]].position - verts_[tri.v[0]].position;
    auto p2 = verts_[tri.v[2]].position - verts_[tri.v[0]].position;
    Vec3 face_normal = p1.cross(p2).normal();
    for (int i: tri.v)
      normal_sum[i] += face_normal;
  }
  for (size_t i = 0; i < verts_.size(); ++i)
    verts_[i].normal = normal_sum[i].normal();
}

void RawModel::translate_to_origin()
{
  Vec3 vsum = Vec3::zero;
  for (const auto& v: verts_)
    vsum += v.position;

  auto center = vsum / verts_.size();
  for (auto& v: verts_)
    v.position -= center;
}

void RawModel::rescale(const bounds::AABB& new_bb, bool par)
{
  auto bb = bounding_box();
  auto w = bb.size();
  for (auto& f: w.v)
    if (f < EPSILON)
      f = 1;

  auto scale = new_bb.size().elem_div(w);
  if (par)
    scale = Vec3{scale.min()};

  for (auto& v: verts_)
  {
    v.position = (v.position - bb.min()).elem_mult(scale) + new_bb.min();
    v.normal = v.normal.elem_div(scale).normal();
  }
}

void RawModel::rescale(const bounds::Sphere& new_bs)
{
  const auto bs = bounding_sphere();

  auto scale = new_bs.radius / bs.radius;
  for (auto& v: verts_)
  {
    v.position = (v.position - bs.center) * scale + new_bs.center;
  }
}

/*
 * A face in a .obj file is 'f' followed by a list of vertex references.
 *
 */
vertex_ref parse_obj_vertex_ref(string token) {

  vertex_ref ref{-1, -1, -1};
  auto pos = token.find('/');
  if (pos == string::npos)
  {
    ref.p = std::stoi(token) - 1;
    return ref;
  }
  ref.p = std::stoi(token.substr(pos)) - 1;

  auto tpos = token.find('/', pos+1);
  if (tpos == string::npos)
  {
    ref.t = std::stoi(token.substr(pos+1, tpos-pos-1)) - 1;
    return ref;
  }

  if (tpos > pos + 1)
  {
    ref.t = std::stoi(token.substr(pos+1, tpos-pos-1)) - 1;
  }
  ref.n = std::stoi(token.substr(tpos+1)) - 1;
  return ref;
}
