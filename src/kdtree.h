#pragma once

#include <memory>
#include <vector>
#include "bounds.h"

using std::shared_ptr;
using std::vector;
using std::pair;

/**
 * This kd-tree implementation is based on the construction method as described
 * in:
 *
 * "Fast kd-tree Construction with an Adaptive Error-Bounded Heuristic" by
 * Hunt, Warren et. al.
 *
 * It uses the SAH heuristic, empty-node cost discounts, and an adpative
 * sampling method for quickly choosing the near-optimal splitting plane heuristic.
 */

namespace kd
{
  struct TreeOptions
  {
    uint num_uniform_samples = 8;
    uint num_adaptive_samples = 8;
    scalar empty_side_discount = 0.85;
    scalar self_traversal_cost = 1.0;

    uint hybrid_one_axis_limit = 1024;
  };

  enum NodeAxis
  {
    X = 0,
    Y,
    Z
  };

  class Node
  {
  private:
    Node(const vector<shared_ptr<Bounded>>& objects, const vector<bounds::AABB>& boxes,
         const bounds::AABB& total_bound,
         const TreeOptions& opt);

    vector<shared_ptr<Bounded>> shapes;
    Node* left, *right;
    bounds::AABB bound;

    struct split_eval
    {
      scalar split, cost, count_diff;
    };

    struct split_plane
    {
      split_plane() = default;
      split_plane(scalar split_, NodeAxis axis_);

      scalar split;
      NodeAxis axis;
    };

    split_eval evaluate_split(const split_plane& sp, const vector<bounds::AABB>& boxes,
                                const TreeOptions& opt, scalar surface_area) const;

    pair<scalar, scalar> child_areas(const bounds::AABB& bound, const split_plane& sp) const;

    ~Node();
  };

  class Tree
  {
  public:
    Tree(const vector<shared_ptr<Bounded>>& objects);

  private:

    shared_ptr<Node> node;
  };
}
