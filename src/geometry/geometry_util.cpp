#include "geometry_util.h"
#include "mesh.h"
#include "kdmesh.h"

unique_ptr<Geometry> mesh_from_model(RawModel&& model)
{
  if (model.tris().size() < 12)
    return make_unique<Mesh>(model);
  else
    return make_unique<KDMesh>(model);
}

unique_ptr<Geometry> load_mesh(const std::string& s)
{
  RawModel model;
  auto status =  model.load(s);

  if (!status.success)
    return nullptr;

  return mesh_from_model(std::move(model));
}

unique_ptr<Geometry> load_mesh(const std::string& s, const bounds::AABB& box)
{
  RawModel model;
  auto status =  model.load(s);

  if (!status.success)
    return nullptr;

  model.rescale(box);

  return mesh_from_model(std::move(model));
}

vector<Vec2> mesh_eval_points(Vec2 xz_min, Vec2 xz_max,
                              uint num_x, uint num_z)
{
  vector<Vec2> points((num_x + 1) * (num_z + 1));

  const scalar dx = (xz_max[0] - xz_min[0]) / num_x;
  const scalar dz = (xz_max[1] - xz_min[1]) / num_z;

  for (auto z = 0u, i = 0u; z <= num_z; ++z)
  {
    const scalar fz = xz_min[1] + z * dz;
    for (auto x = 0u; x <= num_x; ++x, ++i)
    {
      const scalar fx = xz_min[0] + x * dx;
      points[i] = Vec2(fx, fz);
    }
  }

  return points;
}

// This version of parametric mesh is the most generic. All of the other
// functions are implemented in terms of it.
unique_ptr<Geometry> parametric_mesh(const Vec3* points,
                                     Vec2 uv_min, Vec2 uv_max,
                                     uint num_u, uint num_v)
{
  auto ep = mesh_eval_points(uv_min, uv_max,
                             num_u, num_v);
  vector<Vertex> vertices( (num_u + 1) * (num_v + 1) );
  for (auto i = 0u; i < ep.size(); ++i)
  {
    vertices[i].position = points[i];
    vertices[i].uv = ep[i];
  }

  vector<Triangle> tris(num_u * num_v * 2);

  for (auto v = 0u, i = 0u; v < num_v; ++v)
   {
    for (auto u = 0u; u < num_u; ++u, i += 2)
    {
      uint start = v * (num_u + 1) + u;
      tris[i] = Triangle({start, start + num_u + 1, start + 1});
      tris[i+1] = Triangle{start + 1, start + num_u + 1, start + num_u + 2};
    }
  }

  RawModel model;
  model.load_raw_model(vertices, tris, false, true);

  return mesh_from_model(std::move(model));

}

unique_ptr<Geometry> parametric_mesh(std::function<Vec3(scalar, scalar)> p_func,
                                     Vec2 uv_min, Vec2 uv_max,
                                     uint num_u, uint num_v)
{
  auto ep = mesh_eval_points(uv_min, uv_max,
                             num_u, num_v);
  vector<Vec3> points(ep.size());

  std::transform(ep.begin(), ep.end(), points.begin(),
                 [&](const Vec2& v) { return p_func(v[0], v[1]); });

  return parametric_mesh(points.data(), uv_min, uv_max, num_u, num_v);
}

unique_ptr<Geometry> heightfield_mesh(const scalar* heights,
                                   Vec2 xz_min, Vec2 xz_max,
                                   uint num_x, uint num_z)
{
  auto eval_points = mesh_eval_points(xz_min, xz_max, num_x, num_z);
  vector<Vec3> points(eval_points.size());

  for (auto i = 0u; i < eval_points.size(); ++i)
  {
    const auto p = eval_points[i];
    points[i] = Vec3(p[0], heights[i], p[1]);
  }

  return parametric_mesh(points.data(), Vec2(0.0, 0.0), Vec2(1.0, 1.0),
                         num_x, num_z);
}

unique_ptr<Geometry> heightfield_mesh(std::function<scalar(scalar, scalar)> height_func,
                                   Vec2 xz_min, Vec2 xz_max,
                                   uint num_x, uint num_z)
{
  auto eval_points = mesh_eval_points(xz_min, xz_max, num_x, num_z);
  vector<Vec3> points(eval_points.size());

  transform(eval_points.begin(), eval_points.end(), points.begin(),
            [&](const auto& v) { return Vec3(v[0], height_func(v[0], v[1]), v[1]); });

  return parametric_mesh(points.data(), Vec2(0.0, 0.0), Vec2(1.0, 1.0),
                         num_x, num_z);
}
