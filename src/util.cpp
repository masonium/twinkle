#include "util.h"
#include <cstdio>
#include <cstdlib>

int num_system_procs()
{
  FILE* f = popen("nproc", "r");
  if (!f)
  {
    return 1;
  }

  int num_procs;
  
  const int BUF_SIZE = 1024;
  char buf[BUF_SIZE];
  size_t num_read = fread(buf, 1, BUF_SIZE, f);
  if (num_read == 0)
    return 1;

  num_procs = atoi(buf);
  if (num_procs <= 0)
    return 1;
  
  pclose(f);

  return num_procs;
}
