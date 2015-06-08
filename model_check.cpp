#include <iostream>
#include "model.h"

using namespace std;

int main(int argc, char** args)
{
  if (argc < 2)
  {
    cerr << "usage: " << args[0] << " [OBJ_FILENAME]" << endl;
    exit(1);
  }

  RawModel m;
  auto mls = m.load_obj_model(args[1]);
  if (!mls.success)
  {
    cout << "Failed to load " << args[1] << endl;
    exit(1);
  }
  cout << "Loaded model with " << m.verts.size() << " vertices and " << m.tris.size() << " tris." << endl;
}
