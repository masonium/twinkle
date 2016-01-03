#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "util.h"
#include "util/images.h"
#include "reinhard.h"

using std::string;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::make_shared;
using std::shared_ptr;

int main(int UNUSED(argc), char** argv)
{
  string filename(argv[1]);

  auto img = load_image(filename);
  cerr << "Image loaded." << endl;
  if (img.width() == 0)
  {
    cerr << "Could not load image " << filename.c_str() << "\n";
  }

  ReinhardLocal::Options opt;
  auto tonemapper = make_shared<ReinhardGlobal>();

  save_image(argv[2], tonemapper->tonemap(img));
}
