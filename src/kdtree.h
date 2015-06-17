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

    uint max_elements_per_leaf = 4;
  };

  enum NodeAxis
  {
    X = 0,
    Y,
    Z
  };

  template <typename T>
  class Tree;

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

  template <typename T>
  class Node
  {
  public:
    ~Node();

    using object_type = T;

    scalar intersect(const Ray& r, scalar max_t, T& hit);

  private:
    Node(const vector<T>& objects, const vector<bounds::AABB>& boxes,
         const bounds::AABB& total_bound,
         const TreeOptions& opt);

    split_eval evaluate_split(const split_plane& sp, const vector<bounds::AABB>& boxes,
                              const TreeOptions& opt, scalar surface_area) const;

    static pair<scalar, scalar> quadratic_interpolate_best_split(
      const split_eval& x, const split_eval& y, const split_eval& z);

    pair<scalar, split_plane> best_plane_adaptive(
      int axis, const vector<bounds::AABB>& boxes,
      const TreeOptions& opt, scalar surface_area) const;

    pair<scalar, split_plane> best_plane_exhaustive(
      int axis, const vector<bounds::AABB>& boxes,
      const TreeOptions& opt, scalar surface_area) const;

    static pair<scalar, scalar> child_areas(const bounds::AABB& bound, const split_plane& sp);

    void make_leaf(const vector<T>& objects);
    void make_split(const vector<T>& objects,
                    const vector<bounds::AABB>& boxes,
                    const TreeOptions& opt,
                    const split_plane& plane);

    int count_leaves() const
    {
      if (!(left || right))
        return 1;
      return (left ? left->count_leaves() : 0) + (right ? right->count_leaves() : 0);
    }

    int count_objs() const
    {
      if (!(left || right))
        return shapes.size();
      return (left ? left->count_objs() : 0) + (right ? right->count_objs() : 0);
    }

    friend class Tree<T>;
    friend class shared_ptr<Node<T>>;

    /*
     * member fields
     */

    vector<T> shapes;
    bounds::AABB bound;
    Node* left, *right;
    split_plane plane;
  };

  template <typename T>
  class Tree
  {
  public:
    using node_type = Node<T>;

    Tree(const vector<T>& objects, const TreeOptions& opt);

    scalar intersect(const Ray& ray, Geometry*& geom);

    int count_leaves() const
    {
      return root->count_leaves();
    }
    int count_objs() const
    {
      return root->count_objs();
    }

  private:

    shared_ptr<Node<T>> root;
  };
}

#include "kdtree.hpp"
