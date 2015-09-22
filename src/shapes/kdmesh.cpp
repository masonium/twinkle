#include "kdmesh.h"

KDMesh::KDMesh(const RawModel& model) : Mesh(model)
{
  auto tri_addresses = vector<const MeshTri*>{tris.size()};
  for (auto i = 0u; i < tris.size(); ++i)
    tri_addresses[i] = &tris[i];

  kd::TreeOptions opt;
  opt.max_elements_per_leaf = 10;
  kd_tree = std::make_unique<tri_tree>(tri_addresses, opt);
}

bounds::AABB KDMesh::get_bounding_box() const
{
  return kd_tree->get_bounding_box();
}

scalar KDMesh::intersect(const Ray& r, scalar max_t, SubGeo& geo) const
{
  MeshTri const* tri = nullptr;
  SubGeo dummy;
  scalar t = kd_tree->intersect(r, max_t, tri, dummy);
  if (t > 0)
    geo = tri - &tris[0];
  return t;
}
