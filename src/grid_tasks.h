#pragma once

#include "film.h"
#include <vector>

using std::vector;

/*
enum class SubtaskGranularity
{
  GRANULARITY_INTERACTIVE = 0,
    GRANULARITY_EFFICIENT = 1
};
*/

struct grid_subtask_options
{
  uint grid_subdivision;
  //SubtaskGranularity granularity;
};

/*
 * Create a set of substasks for parallel computation of a grid, that can be
 * used later for scheduling.
 */


vector<Film::Rect> subtasks_from_grid(uint width, uint height, uint threads);
vector<Film::Rect> subtasks_from_grid(uint width, uint height, const grid_subtask_options& opt);
