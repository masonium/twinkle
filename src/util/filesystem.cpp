#include <util/filesystem.h>
#include <fstream>

namespace filesystem
{
  bool exists(const std::string& filename)
  {
    std::ifstream ifs(filename.c_str());
    return ifs;
  }
}
