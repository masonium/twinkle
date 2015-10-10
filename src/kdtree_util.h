#pragma once

#include <cstdint>
#include <iostream>
#include "twinkle.h"

using std::ostream;

namespace kd
{
  struct TreeOptions
  {
    uint32_t num_uniform_samples = 8;
    uint32_t num_adaptive_samples = 8;
    scalar empty_side_discount = 0.85;
    scalar self_traversal_cost = 1.0;

    uint32_t hybrid_one_axis_limit = 1024;

    uint32_t exact_evaluation_limit = 36;

    uint32_t max_elements_per_leaf = 4;
  };

  enum NodeAxis
  {
    X = 0,
    Y,
    Z,
    NONE
  };

  struct split_eval
  {
    scalar split, cost, count_diff;
  };

  struct split_plane
  {
    split_plane() = default;
    split_plane(scalar split_, NodeAxis axis_);

    bool operator <(const split_plane& rhs) const;

    scalar split;
    NodeAxis axis;
  };

  ostream& operator <<(ostream&, const split_plane&);
}
