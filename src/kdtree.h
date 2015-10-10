#pragma once

#include <memory>
#include <vector>
#include <stack>
#include <type_traits>
#include "bounds.h"
#include "geometry.h"
#include "kdtree_util.h"

using std::unique_ptr;
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
    using element_type = T const*;

    Tree(const vector<element_type>& objects, const TreeOptions& opt);

    scalar_fp intersect(const Ray& ray, const scalar_fp max_t, element_type& geom, SubGeo& subgeo) const;

    auto get_bounding_box() const { return bound; }

    int height() const
    {
      return root ? root->heigh() : 0;
    }

    int count_leaves() const
    {
      return root ? root->count_leaves() : 0;
    }
    int count_objs() const
    {
      return root ? root->count_objs() : 0;
    }

  private:
    unique_ptr<Node<T>> root;
    bounds::AABB bound;
    uint _height;
  };

  template <typename T>
  class Node
  {
  public:
    ~Node();

    using element_type = T const*;

  private:
    Node(const vector<element_type>& objects, const vector<bounds::AABB>& boxes,
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

    void make_leaf(const vector<element_type>& objects);
    void make_split(const vector<element_type>& objects, const vector<bounds::AABB>& boxes,
                    const bounds::AABB& bound, const split_plane& plane,
                    const TreeOptions& opt);

    /**
     * intersection methods
     */
    bool is_leaf() const {
      return left == nullptr && right == nullptr;
    }

    scalar_fp leaf_intersect(const Ray& ray, scalar_fp max_t, element_type& obj, SubGeo& geo) const;

    /**
     * statistic methods
     */
    int height() const
    {
      return 1 + (left ? left->height() : 0) + (right ? right->height() : 0);
    }

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
    friend class unique_ptr<Node<T>>;

    /*
     * member fields
     */
    vector<element_type> shapes;
    Node *left, *right;
    split_plane plane;
  };
}

#include "kdtree.hpp"
