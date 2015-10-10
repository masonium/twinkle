#include <algorithm>
#include <iterator>
#include <memory>
#include <cassert>
#include <stack>
#include "kdtree.h"
#include "math_util.h"

using std::max_element;
using std::make_unique;
using std::distance;
using std::count_if;
using std::tie;
using std::move;
using std::make_pair;
using std::make_tuple;
using std::cout;
using std::cerr;
using std::endl;

namespace kd
{

  template <typename T>
  Node<T>::Node(const vector<element_type>& objects,
                const vector<bounds::AABB>& boxes,
                const bounds::AABB& total_bound, const TreeOptions& opt)
    : left(nullptr), right(nullptr)
  {
    const auto num_boxes = boxes.size();
    if (num_boxes <= opt.max_elements_per_leaf)
    {
      make_leaf(objects);
      return;
    }

    auto bb_size = total_bound.size();
    int longest_axis = distance(bb_size.v,max_element(bb_size.v, bb_size.v + 3));

    const scalar sa = total_bound.surface_area();

    pair<scalar, split_plane> best_split = make_pair(SCALAR_MAX, split_plane{0, X});

    // Search some subset of the axes for the best split plane.
    for (int ai = 0; ai < 3; ++ai)
    {
      if (bb_size[ai] == 0)
        continue;

      if (num_boxes > opt.hybrid_one_axis_limit && ai != longest_axis)
        continue;

      auto x = num_boxes <= opt.exact_evaluation_limit ?
        best_plane_exhaustive(ai, boxes, total_bound, sa, opt) :
        best_plane_adaptive(ai, boxes, total_bound, sa, opt);

      best_split = min(x, best_split);
    }

    const scalar no_split_cost = opt.empty_side_discount * num_boxes;

    // Compare the best split found to not splitting at all
    if (best_split.first < no_split_cost)
    {
      // std::cerr << "cost of best split " << best_split.second << " = "
      // << best_split.first << " beats no split cost of "
      //           << no_split_cost << std::endl;
      make_split(objects, boxes, total_bound, best_split.second, opt);
    }
    else
    {
      // cout << "cost " << best_split.first << " exceeds number of boxes "
      //      << num_boxes << "\n";
      make_leaf(objects);
    }
  }

  /*
   * Given three split evaluations (increasing monotonically by split value),
   * fit the split cost along the interval to a quadratic function, and return
   * the global minimum on the interval.
   */
  template <typename T>
  pair<scalar, scalar> Node<T>::quadratic_interpolate_best_split(
    const split_eval& x, const split_eval& y, const split_eval& z)
  {
    Vec3 coef = interpolate_quadratic(x.split, x.cost, y.split, y.cost, z.split, z.cost);

    // If the quadratic coefficient is zero or negative, the minimum lies on an
    // endpoint.
    if (coef[0] > EPSILON)
    {
      auto min_split = -coef[1] / (2 * coef[0]);
      if (x.split < min_split && min_split < z.split)
      {
        return make_pair((coef[0] * min_split + coef[1]) * min_split + coef[2], min_split);
      }
    }

    if (z.cost < x.cost)
      return make_pair(z.cost, z.split);
    else
      return make_pair(x.cost, x.split);
  }

  template <typename T>
  pair<scalar, split_plane> Node<T>::best_plane_exhaustive(int axis, const vector<bounds::AABB>& boxes,
                                                           const bounds::AABB& bound, scalar surface_area,
                                                           const TreeOptions& opt) const
  {
    vector<scalar> splits;
    transform(boxes.begin(), boxes.end(), std::inserter(splits, splits.end()),
              [=](const auto& b) { return b.bounds[0][axis];});
    transform(boxes.begin(), boxes.end(), std::inserter(splits, splits.end()),
              [=](const auto& b) { return b.bounds[1][axis];});

    scalar best_split = -1;
    scalar best_split_cost = SCALAR_MAX;

    for (scalar split: splits)
    {
      auto eval = evaluate_split(split_plane{split, kd::NodeAxis(axis)}, boxes, bound, surface_area, opt);
      if (best_split_cost > eval.cost)
      {
        best_split_cost = eval.cost;
        best_split = split;
      }
    }

    return make_pair(best_split_cost, split_plane{best_split, kd::NodeAxis(axis)});
  }

  /**
   * Evaluate splits at specifically-chosen points to determine an approximation
   * to the best possible split.
   */
  template <typename T>
  pair<scalar, split_plane> Node<T>::best_plane_adaptive(int axis, const vector<bounds::AABB>& boxes,
                                                         const bounds::AABB& bound, scalar surface_area,
                                                         const TreeOptions& opt) const
  {
    auto naxis = static_cast<NodeAxis>(axis);

    auto num_samples = opt.num_uniform_samples + opt.num_adaptive_samples;
    auto split_evals = vector<split_eval>(num_samples);

    /*
     * 1. Uniform sampling.
     *
     * Sample uniformly along the axis.
     */
    auto bb_size = bound.size();
    for (auto i = 0u; i < opt.num_uniform_samples; ++i)
    {
      auto sp = split_plane{bound.min()[axis] + bb_size[axis] * i / (opt.num_uniform_samples - 1), naxis};

      split_evals[i] = evaluate_split(sp, boxes, bound, surface_area, opt);
    }

    /*
     * 2. Adaptive Sampling
     *
     * Sample between the uniform points so that the number of samples between
     * two consecutive uniform points is roughly proportional to the size of
     * the difference in count differences between those ponits.
     *
     */
    scalar min_count_diff = (*split_evals.begin()).count_diff;
    scalar max_count_diff = (*split_evals.rbegin()).count_diff;
    scalar cd_range = max_count_diff - min_count_diff;

    /*
     * 2a. Computing samples per bin
     *
     * We sample uniformly in the count-difference space. Then, we map these
     * points (via piecewise linear interpolation) back to the axis space.
     *
     * We bin this mapped points, using the uniform samples as binpoints.
     *
     * (An alternative strategy would be simply to sample from a multinomial
     * distribution using the relative differences as bin probabilities. This
     * would probably reduce bias, but at the cost of higher variance since
     * the sample sizes are so small.)
     */
    vector<scalar> num_inner_samples(opt.num_adaptive_samples - 1, 0);
    int split_idx = 0;
    for (auto i = 0u; i < opt.num_adaptive_samples; ++i)
    {
      scalar f = scalar(i+1) / (opt.num_adaptive_samples + 1);
      scalar split_diff = min_count_diff + cd_range * f;

      while (split_evals[split_idx+1].count_diff < split_diff)
      {
        // We're guaranteed that split_diff will never be larger than
        // max_count_diff, since f < 1
        ++split_idx;
      }

      ++num_inner_samples[split_idx];
    }

    /*
     * 2b. Uniformly sample on the split axis within each bin, according to
     * our previous sample counts.
     *
     * Since we know precisely the final order of the samples, we can move the
     * existing samples to correct position in the final array as we go.
     */
    split_evals[num_samples - 1] = split_evals[opt.num_uniform_samples];
    for (int i = num_inner_samples.size() - 1, ei = num_samples - 2; i >= 0; --i, --ei)
    {
      for (int j = num_inner_samples[i] - 1; j >= 0; --j, --ei)
      {
        scalar f = (j+1) / (num_inner_samples[i] + 1);
        scalar split = split_evals[i].split * (1-f) + split_evals[i+1].split * f;
        split_evals[ei] = evaluate_split(split_plane{split, naxis}, boxes, bound, surface_area, opt);
      }
      split_evals[ei] = split_evals[i];
    }

    /*
     * 3. Piece-wise quadratic interpolation to find the optimal place.
     */
    pair<scalar, scalar> axis_best_split =
      quadratic_interpolate_best_split(split_evals[0], split_evals[1], split_evals[2]);

    for (uint i = 1; i < split_evals.size() - 3; ++i)
    {
      auto split = quadratic_interpolate_best_split(
        split_evals[i], split_evals[i+1], split_evals[i+2]);

      axis_best_split = min(split, axis_best_split);
    }

    return make_pair(axis_best_split.first, split_plane{axis_best_split.second, naxis});
  }

  template <typename T>
  pair<scalar, scalar> Node<T>::child_areas(const bounds::AABB& bound,
                                            const split_plane& sp)
  {
    auto bb_size = bound.size();
    const auto s1 = bb_size[(sp.axis+1) % 3];
    const auto s2 = bb_size[(sp.axis+2) % 3];
    const auto perp_area = 2 * (s1 + s2) * bb_size[sp.axis];
    const auto split_area = 2 * s1 * s2;

    const auto left_p = (sp.split - bound.min()[sp.axis]) / bb_size[sp.axis];
    return make_pair(split_area + left_p * perp_area, split_area + (1 - left_p) * perp_area);
  }

  /**
   * Return the total cost associated with spliting this node at the supplied
   * split_plane.
   */
  template <typename T>
  split_eval Node<T>::evaluate_split(const split_plane& sp, const vector<bounds::AABB>& boxes,
                                     const bounds::AABB& bound, scalar surface_area,
                                     const TreeOptions& opt) const
  {
    int num_middle = count_if(boxes.begin(), boxes.end(),
                              [&sp](const auto& bb)
                              {
                                return bb.hits_plane(sp.axis, sp.split);
                              });

    int num_left = count_if(boxes.begin(), boxes.end(),
                            [&sp](const auto& bb)
                            {
                              return bb.max()[sp.axis] < sp.split;
                            });
    int num_right = boxes.size() - num_left;
    num_left += num_middle;

    scalar sa_left = 0, sa_right = 0;
    tie(sa_left, sa_right) = child_areas(bound, sp);

    scalar cost = opt.self_traversal_cost + (num_left * sa_left + num_right * sa_right) / surface_area;

    if (num_left == 0 || num_right == 0)
      cost *= opt.empty_side_discount;

    return split_eval{sp.split, cost, static_cast<scalar>(num_left - num_right)};
  }

  /*
   * copy the object references into the leaf.
   */
  template <typename T>
  void Node<T>::make_leaf(const vector<element_type>& objects)
  {
    shapes.resize(objects.size());
    std::copy(objects.begin(), objects.end(), shapes.begin());
  }

  template <typename T>
  void Node<T>::make_split(const vector<element_type>& objects, const vector<bounds::AABB>& boxes,
                           const bounds::AABB& bound, const split_plane& sp,
                           const TreeOptions& opt)
  {
    vector<element_type> left_objects, right_objects;
    vector<bounds::AABB> left_boxes, right_boxes;

    this->plane = sp;

    auto left_bound = bound, right_bound = bound;
    left_bound.max()[sp.axis] = sp.split;
    right_bound.min()[sp.axis] = sp.split;

    for (auto i = 0u; i < objects.size(); ++i)
    {
      bool add_left = true, add_right = true;
      if (boxes[i].max()[sp.axis] < sp.split)
        add_right = false;
      if (boxes[i].min()[sp.axis] > sp.split)
        add_left = false;

      if (add_left)
      {
        left_objects.push_back(objects[i]);
        auto left = boxes[i];
        left.max()[sp.axis] = std::min(left.max()[sp.axis], sp.split);
        left_boxes.push_back(left);
      }

      if (add_right)
      {
        right_objects.push_back(objects[i]);
        auto right = boxes[i];
        right.min()[sp.axis] = std::max(right.min()[sp.axis], sp.split);
        right_boxes.push_back(right);
      }
    }

    // std::cerr << "Making split on " << sp << " (" << bound.min()[sp.axis] << ", " << bound.max()[sp.axis] << "): " << boxes.size() << ", "
    //           << left_boxes.size() << ", " << right_boxes.size() << std::endl;

    if (!left_objects.empty())
      left = new Node(left_objects, left_boxes, left_bound, opt);

    if (!right_objects.empty())
      right = new Node(right_objects, right_boxes, right_bound, opt);
  }



  template <typename T>
  scalar_fp Node<T>::leaf_intersect(const Ray& ray, scalar_fp max_t, element_type& obj, SubGeo& geo) const
  {
    scalar_fp best_t = max_t;
    element_type best_obj{nullptr};
    SubGeo best_geo = 0, leaf_geo = 0;

    for (const auto& shape: shapes)
    {
      auto t = shape->intersect(ray, best_t, leaf_geo);
      if (t < best_t)
      {
        best_t = t;
        best_obj = shape;
        best_geo = leaf_geo;
      }
    }

    if (best_obj != nullptr)
    {
      obj = best_obj;
      geo = best_geo;
      return best_t;
    }

    return none_tag;
  }

  template <typename T>
  Node<T>::~Node()
  {
    if (left)
      delete left;
    if (right)
      delete right;
  }

////////////////////////////////////////////////////////////////////////////////
  template<typename T>
  Tree<T>::Tree(const vector<element_type>& objects, const TreeOptions& opt) :
    root(nullptr), bound(Vec3::zero, Vec3::zero), _height(0)
  {
    if (!objects.empty())
    {
      auto boxes = vector<bounds::AABB>(objects.size());
      transform(objects.begin(), objects.end(), boxes.begin(),
                [](const auto& a) { return a->get_bounding_box(); });

      bound = accumulate(boxes.begin() + 1, boxes.end(), boxes[0], bounds::AABB::box_union);

      root = unique_ptr<node_type>(new node_type(objects, boxes, bound, opt));
      _height = root->height();
    }
  }

  template <typename T>
  scalar_fp Tree<T>::intersect(const Ray& ray, const scalar_fp max_t, element_type& obj, SubGeo& geo) const
  {
    scalar t0, t1;
    if (!bound.intersect(ray, t0, t1))
      return sfp_none;

    assert(t0 <= t1);

    if (t1 <= 0 || max_t <= t0)
      return sfp_none;

    t0 = std::max<scalar>(t0, 0);
    if (max_t.is())
      t1 = std::min(t1, max_t.get());
    assert(0 <= t0);
    assert(t0 <= t1);
    assert(t1 <= max_t);

    using stack_elem = std::tuple<const Node<T>*, scalar, scalar>;

    std::vector<stack_elem> stack_source(_height);
    std::stack<stack_elem, std::vector<stack_elem> > node_stack(stack_source);

    const Node<T>* active = root.get();

    while (t0 < max_t)
    {
      // if we push an empty node, just continue
      if (active == nullptr)
      {
      }
      /**
       * When we hit a leaf, pick the best intersection of objects within the
       * leaf, if any.
       **/
      else if (active->is_leaf())
      {
        auto best_t = active->leaf_intersect(ray, max_t, obj, geo);
        if (best_t.is())
          return best_t;
      }
      /**
       * Otherwise, use the split position to find which children, if any, to
       * push onto the stack.
       */
      else {
        scalar t_split = (active->plane.split - ray.position[active->plane.axis])
          * ray.inv_direction[active->plane.axis];
        Node<T> *first = active->left, *second = active->right;
        if (ray.direction[active->plane.axis] < 0)
          std::swap(first, second);


        if (t0 < t_split && t_split < t1)
        {
          node_stack.emplace(second, t_split, t1);
          active = first;
          t1 = t_split;
          continue;
        }
        else if (t_split < t0)
        {
          active = second;
          continue;
        }
        else
        {
          active = first;
          continue;
        }
      }
      if (node_stack.empty())
        break;

      tie(active, t0, t1) = node_stack.top();
      node_stack.pop();
    }

    return sfp_none;
  }
}
