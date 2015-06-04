#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "film.h"

using std::string;
using std::ifstream;
using std::cout;
using std::endl;
using std::make_shared;

int main(int argc, char** args)
{
  assert( argc >= 2 );
  string filename{args[1]};

  ifstream f(filename, std::ios::in | std::ios::binary);

  Film film(f);
  //film.render_to_ppm(cout, new LinearToneMapper);
  film.render_to_ppm(cout, make_shared<RSSFToneMapper>(0.18));
}


// int main()
// {
//   spectrum s{2.0, 3.0, 1.0};

//   cout << "color: " << s << endl;
//   cout << "old luminance: " << s.luminance() << endl;
//   spectrum n = s.rescale_luminance(1.0);
//   cout << "new spectrum: " << n << endl;
//   cout << "new luminance: " << n.luminance() << endl;
// }
