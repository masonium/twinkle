#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "film.h"
#include "reinhard.h"

using std::string;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::make_shared;
using std::shared_ptr;

int main(int argc, char** argv)
{
  string filename(argv[0]);

  ifstream f(filename, std::ios::in | std::ios::binary);
  Film film(f);

  auto tonemap = make_shared<ReinhardGlobal>(0.18);

  film.render_to_ppm(cout, *tonemap);
}
