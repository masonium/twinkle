#include <memory>
#include <vector>
#include "quad.h"
#include "mesh.h"

using std::shared_ptr;
using std::make_shared;
using std::vector;

shared_ptr<Mesh> make_quad()
{
  return make_quad( Vec3::x_axis, Vec3::y_axis );
}


shared_ptr<Mesh> make_quad(const Vec3& xv, const Vec3& yv)
{
  vector<Vertex> vertices(4);
  Vec3 pos[4] = {-xv -  yv, xv - yv, -xv + yv, xv + yv};
  Vec2 uvs[4] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
  for (auto i = 0u; i < 4; ++i)
  {
    vertices[i].position = pos[i];
    vertices[i].uv = uvs[i];
  }
  vector<Triangle> tris{{0, 1, 2}, {2, 1, 3}};

  RawModel model;
  model.load_raw_model(vertices, tris, false, true);
  return make_shared<Mesh>(model);
}
