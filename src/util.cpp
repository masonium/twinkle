#include "util.h"
#include <cstdio>
#include <cstdlib>
#include <thread>

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
