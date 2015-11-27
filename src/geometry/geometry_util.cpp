#include "geometry_util.h"
#include "mesh.h"
#include "kdmesh.h"

unique_ptr<Geometry> mesh_from_model(RawModel&& model)
{
  if (model.verts().size() < 12)
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
