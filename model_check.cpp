#include <iostream>
#include "model.h"
#include "material.h"
#include "sphere.h"
#include "spectrum.h"
#include "kdtree.h"

using namespace std;

void kdtree_test()
{
  auto mat = make_shared<RoughColorMaterial>(0.0, spectrum{1.0});

  auto shapes = vector<shared_ptr<Bounded>>();

  UniformSampler s;

  for (int i = 0; i < 1000; ++i)
  {
    auto sp = make_shared<Sphere>(Vec3{s.ruf(), s.ruf(), s.ruf()}, s.ruf() * 0.3f);
    shapes.push_back(sp);
  }

  kd::TreeOptions opt;
  opt.self_traversal_cost = 20.0;
  auto tree = make_shared<kd::Tree>(shapes, opt);
  cout << "built tree with " << tree->count_leaves() << " leaves and " << tree->count_objs() << " obj refs.\n";
}

int main(int argc, char** args)
{
  // if (argc < 2)
  // {
  //   cerr << "usage: " << args[0] << " [OBJ_FILENAME]" << endl;
  //   exit(1);
  // }

  // RawModel m;
  // auto mls = m.load_obj_model(args[1]);
  // if (!mls.success)
  // {
  //   cout << "Failed to load " << args[1] << endl;
  //   exit(1);
  // }
  // cout << "Loaded model with " << m.verts.size() << " vertices and " <<
  // m.tris.size() << " tris." << endl;

  kdtree_test();
}
