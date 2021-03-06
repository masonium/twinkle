#include "grid_tasks.h"

using std::min;

vector<Film::Rect> subtasks_from_grid(uint width, uint height, const grid_subtask_options& opt)
{
  vector<Film::Rect> subtasks;

  auto gs = min<uint>(min(width, height), max<uint>(opt.grid_subdivision, 1));

  for (uint i = 0; i < gs; ++i)
  {
    uint x = width * i / gs;
    uint w = width * (i+1) / gs - width * i / gs;

    if (w == 0)
      continue;

    for (uint j = 0; j < gs; ++j)
    {
      uint y = height * j / gs;
      uint h = height * (j+1) / gs - height * j / gs;

      if (h == 0)
        continue;

      assert(x + w <= width);
      assert(y + h <= height);

      subtasks.emplace_back(x, y, w, h);
    }
  }

  return subtasks;
}

vector<Film::Rect> subtasks_from_grid(uint width, uint height, uint num_threads)
{
  grid_subtask_options opt;

  if (num_threads == 1)
    opt.grid_subdivision = 1;
  else
    opt.grid_subdivision = max<uint>(2, min(width, height) / 32);

  return subtasks_from_grid(width, height, opt);
}
