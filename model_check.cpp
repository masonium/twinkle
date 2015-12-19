#include <iostream>
#include "model.h"
#include "scheduler.h"
#include "material.h"
#include "textures.h"
#include "spectrum.h"
#include "kdtree.h"
#include "geometries.h"

using namespace std;

void quad_test()
{
  auto mesh = make_quad();

  cout << mesh->n(0) << endl;
}

void scheduler_test()
{
  class SleepTask : public LocalTask
  {
  public:
    SleepTask(int index_) : index(index_) {}
    int index;
    void run(uint task) override
    {
      cout << "starting task " << index << "\n";
      std::this_thread::sleep_for(std::chrono::seconds(2));
      cout << "finishing task " << index << "\n";
    }
  };

  auto scheduler = make_scheduler(4u);

  int j = 0;
  for (auto i = 0; i < 9; ++i)
    scheduler->add_task(make_shared<SleepTask>(++j));

  scheduler->complete_pending();
  cout << "Round 2:" << endl;

  for (auto i = 0; i < 9; ++i)
    scheduler->add_task(make_shared<SleepTask>(++j));

  scheduler->complete_pending();
}

void kdtree_test()
{
  auto mat = make_shared<RoughColorMaterial>(0.0, spectrum{1.0});

  auto shapes = vector<shared_ptr<Sphere>>();

  UniformSampler s;

  for (int i = 0; i < 1000; ++i)
  {
    auto sp = make_shared<Sphere>(Vec3{s.ruf(), s.ruf(), s.ruf()}, s.ruf() * 0.1f);
    shapes.push_back(sp);
  }

  vector<const Sphere*> spheres(shapes.size());
  std::transform(shapes.begin(), shapes.end(), spheres.begin(),
                 [](auto& a) { return a.get(); } );

  kd::TreeOptions opt;
  opt.self_traversal_cost = 1.0;
  auto tree = make_shared<kd::Tree<Sphere>>(spheres, opt);
  cout << "built tree with " << tree->count_leaves() << " leaves and " << tree->count_objects() << " obj refs.\n";
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

  //kdtree_test();
  //quad_test();
  scheduler_test();
}
