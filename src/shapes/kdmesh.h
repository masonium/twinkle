#include "mesh.h"
#include "kdtree.h"

using tri_tree = kd::Tree<MeshTri const*>;

class KDMesh : public Mesh
{
public:
  KDMesh(const RawModel& model);

  scalar intersect(const Ray& r, scalar max_t, const Geometry*& geom) const override;

  virtual ~KDMesh() {}

  KDMesh(const KDMesh&) = delete;
  KDMesh& operator=(const KDMesh&) = delete;

  friend class MeshTri;
  shared_ptr<tri_tree> kd_tree;
};
