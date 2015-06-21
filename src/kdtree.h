#pragma once

#include <memory>
#include <vector>
#include <stack>
#include <type_traits>
#include "bounds.h"
#include "geometry.h"
#include "kdtree_util.h"

using std::shared_ptr;
using std::vector;
using std::pair;
using std::stack;

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
  template <typename T>
  class Node;

  template <typename T>
  class Tree
  {
  public:
    using node_type = Node<T>;
    using element_type = T;
    using const_element_type = typename std::remove_const<T>::type;

    Tree(const vector<T>& objects, const TreeOptions& opt);

    scalar intersect(const Ray& ray, scalar max_t, Geometry const*& geom) const;

    int count_leaves() const
    {
      return root->count_leaves();
    }
    int count_objs() const
    {
      return root->count_objs();
    }

  private:
    bounds::AABB bound;
    shared_ptr<Node<T>> root;
  };

  template <typename T>
  class Node
  {
  public:
    ~Node();

    using object_type = T;

  private:
    Node(const vector<T>& objects, const vector<bounds::AABB>& boxes,
         const bounds::AABB& total_bound,
         const TreeOptions& opt);

    /**
     * construction methods
     **/
    split_eval evaluate_split(const split_plane& sp, const vector<bounds::AABB>& boxes,
                              const bounds::AABB& bound, scalar surface_area,
                              const TreeOptions& opt) const;

    static pair<scalar, scalar> quadratic_interpolate_best_split(
      const split_eval& x, const split_eval& y, const split_eval& z);

    pair<scalar, split_plane> best_plane_adaptive(
      int axis, const vector<bounds::AABB>& boxes,
      const bounds::AABB& bound, scalar surface_area,
      const TreeOptions& opt) const;

    pair<scalar, split_plane> best_plane_exhaustive(
      int axis, const vector<bounds::AABB>& boxes,
      const bounds::AABB& bound, scalar surface_area,
      const TreeOptions& opt) const;

    static pair<scalar, scalar> child_areas(const bounds::AABB& bound, const split_plane& sp);

    void make_leaf(const vector<T>& objects);
    void make_split(const vector<T>& objects, const vector<bounds::AABB>& boxes,
                    const bounds::AABB& bound, const split_plane& plane,
                    const TreeOptions& opt);

    /**
     * intersection methods
     */
    bool is_leaf() const {
      return left == nullptr && right == nullptr;
    }

    /**
     * statistic methods
     */
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

    /**
     * friends
     */
    friend class Tree<T>;
    friend class shared_ptr<Node<T>>;

    /*
     * member fields
     */

    vector<T> shapes;
    Node *left, *right;
    split_plane plane;
  };
}

#include "kdtree.hpp"
