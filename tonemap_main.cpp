#include <iostream>
#include <fstream>
#include <string>
#include "film.h"

using std::string;
using std::ifstream;
using std::cout;

int main(int argc, char** args)
{
  assert( argc >= 2 );
  string filename{args[1]};

  ifstream f(filename, std::ios::in | std::ios::binary);

  Film film(f);
  film.render_to_ppm(cout, new LinearToneMapper);
}
