#include "util.h"
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <cstdio>

using std::string;

class ProcWrapper
{
public:
  explicit ProcWrapper(FILE* f_) : f(f_)
  {
  }

  operator bool () { return static_cast<bool>(f); }
  operator FILE* () { return f;}

  ~ProcWrapper() {
    if (f)
      pclose(f);
  }
private:
  FILE* f;
};

uint num_system_procs()
{
  return std::thread::hardware_concurrency();
}

string lowercase(const string& s)
{
  string ret;
  ret.resize(s.size());
  std::transform(s.begin(), s.end(), ret.begin(), tolower);

  return ret;
}
