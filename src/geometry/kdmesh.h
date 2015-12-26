#include "mesh.h"
#include "kdtree.h"

using std::unique_ptr;
using tri_tree = kd::Tree<MeshTri>;

class KDMesh : public Mesh
{
public:
  KDMesh(const RawModel& model);

  scalar_fp intersect(const Ray& r, scalar_fp max_t, SubGeo& geom) const override;

  bounds::AABB get_bounding_box() const override;

  virtual string to_string() const override { return "KDMesh"; }

  virtual ~KDMesh() {}

  KDMesh(const KDMesh&) = delete;
  KDMesh& operator=(const KDMesh&) = delete;

  friend class MeshTri;
  unique_ptr<tri_tree> kd_tree;
};
