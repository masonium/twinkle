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
    using obj_index = uint32_t;
    using element_offset_t = uint32_t;

    Tree(const vector<element_type>& objects, const TreeOptions& opt);

    scalar_fp intersect(const Ray& ray, const scalar_fp max_t, element_type& geom, SubGeo& subgeo) const;

    auto get_bounding_box() const { return bound; }

    int height() const
    {
      return _height;
    }

    int count_leaves() const
    {
      return root->count_leaves();
    }
    int count_objs() const
    {
      return root->count_objs();
    }

  private:
    vector<element_type> elements;
    vector<obj_index> indices;

    scalar_fp leaf_intersect(const Node<T>& leaf,
                             const Ray& ray, scalar_fp max_t, element_type& obj, SubGeo& geo) const;

    element_offset_t add_node_indices(const vector<obj_index>&);

    friend class Node<T>;

    unique_ptr<Node<T>> root;
    bounds::AABB bound;
    size_t  _height;
  };

  template <typename T>
  class Node
  {
  public:
    ~Node();

    using element_index = uint32_t;
    using tree_t = Tree<T>;

  private:
    Node(Tree<T>& _parent,
         const vector<element_index>& objects,
         const vector<bounds::AABB>& boxes,
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

    void make_leaf(Tree<T>&, const vector<element_index>& objects);
    void make_split(Tree<T>&, const vector<element_index>& objects,
                    const vector<bounds::AABB>& boxes,
                    const bounds::AABB& bound, const split_plane& plane,
                    const TreeOptions& opt);

    /**
     * intersection methods
     */
    bool is_leaf() const {
      return left == nullptr;
    }

    /**
     * statistic methods
     */
    size_t height() const
    {
      return 1 + (left ? std::max(left->height(), right->height()) : 0);
    }

    size_t count_leaves() const
    {
      if (!left)
        return 1;
      return left->count_leaves() + right->count_leaves();
    }

    size_t count_objs() const
    {
      if (!left)
        return num_objects;
      return left->count_objs() + right->count_objs();
    }

    /**
     * friends
     */
    friend class Tree<T>;
    friend class unique_ptr<Node<T>>;

    /*
     * Leaf member fields
     */
    uint32_t num_objects;
    uint32_t offset;

    /*
     * Inner node member fields
     */
    Node *left, *right;
    split_plane plane;
  };
}

#include "kdtree.hpp"
