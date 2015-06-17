#include <algorithm>
#include <iterator>
#include <memory>
#include <cassert>
#include "kdtree.h"
#include "math_util.h"

using std::max_element;
using std::make_shared;
using std::distance;
using std::count_if;
using std::tie;
using std::move;
using std::make_pair;
using std::cout;
using std::endl;

namespace kd
{

  Node::Node(const vector<shared_ptr<Bounded>>& objects,
             const vector<bounds::AABB>& boxes,
             const bounds::AABB& total_bound, const TreeOptions& opt)
    : bound(total_bound), left(nullptr), right(nullptr)
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

      auto x = best_plane_adaptive(ai, boxes, opt, sa);
      best_split = min(x, best_split);
    }

    // Compare the best split found to not splitting at all
    if (best_split.first < num_boxes)
      make_split(objects, boxes, opt, best_split.second);
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
  pair<scalar, scalar> Node::quadratic_interpolate_best_split(
    const Node::split_eval& x, const Node::split_eval& y, const Node::split_eval& z)
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

  /**
   *
   */
  pair<scalar, Node::split_plane> Node::best_plane_adaptive(int axis, const vector<bounds::AABB>& boxes,
                                             const TreeOptions& opt, scalar surface_area) const
  {
    auto naxis = static_cast<NodeAxis>(axis);

    auto num_samples = opt.num_uniform_samples + opt.num_adaptive_samples;
    auto split_evals = vector<Node::split_eval>(num_samples);

    /*
     * 1. Uniform sampling.
     *
     * Sample uniformly along the axis.
     */
    auto bb_size = bound.size();
    for (auto i = 0u; i < opt.num_uniform_samples; ++i)
    {
      auto sp = split_plane{bound.min[axis] + bb_size[axis] * i / (opt.num_uniform_samples - 1), naxis};

      split_evals[i] = evaluate_split(sp, boxes, opt, surface_area);
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
        split_evals[ei] = evaluate_split(split_plane{split, naxis}, boxes, opt, surface_area);
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

  pair<scalar, scalar> Node::child_areas(const bounds::AABB& bound,
                                         const Node::split_plane& sp)
  {
    auto bb_size = bound.size();
    const auto s1 = bb_size[(sp.axis+1) % 3];
    const auto s2 = bb_size[(sp.axis+2) % 3];
    const auto perp_area = 2 * (s1 + s2);
    const auto split_area = 2 * s1 * s2;

    const auto left_p = (sp.split - bound.min[sp.axis]) / bb_size[sp.axis];
    return make_pair(split_area + left_p * perp_area, split_area + (1 - left_p) * perp_area);
  }

  /**
   * Return the total cost associated with spliting this node at the supplied
   * split_plane.
   */
  Node::split_eval Node::evaluate_split(const split_plane& sp, const vector<bounds::AABB>& boxes,
                                        const TreeOptions& opt, scalar surface_area) const
  {
    int num_middle = count_if(boxes.begin(), boxes.end(),
                              [&sp](const auto& bb)
                              {
                                return bb.hits_plane(sp.axis, sp.split);
                              });

    int num_left = count_if(boxes.begin(), boxes.end(),
                            [&sp](const auto& bb)
                            {
                              return bb.max[sp.axis] < sp.split;
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
  void Node::make_leaf(const vector<shared_ptr<Bounded>>& objects)
  {
    shapes.resize(objects.size());
    std::copy(objects.begin(), objects.end(), shapes.begin());
  }

  void Node::make_split(const vector<shared_ptr<Bounded>>& objects,
                        const vector<bounds::AABB>& boxes,
                        const TreeOptions& opt, const split_plane& sp)
  {
    vector<shared_ptr<Bounded>> left_objects, right_objects;
    vector<bounds::AABB> left_boxes, right_boxes;

    this->plane = sp;

    auto left_bound = bound, right_bound = bound;
    left_bound.max[sp.axis] = sp.split;
    right_bound.min[sp.axis] = sp.split;

    for (auto i = 0u; i < objects.size(); ++i)
    {
      bool add_left = true, add_right = true;
      if (boxes[i].max[sp.axis] < sp.split)
        add_right = false;
      if (boxes[i].min[sp.axis] > sp.split)
        add_left = false;

      if (add_left)
      {
        left_objects.push_back(objects[i]);
        auto left = boxes[i];
        left.max[sp.axis] = std::min(left.max[sp.axis], sp.split);
        left_boxes.push_back(left);
      }

      if (add_right)
      {
        right_objects.push_back(objects[i]);
        auto right = boxes[i];
        right.min[sp.axis] = std::max(right.min[sp.axis], sp.split);
        right_boxes.push_back(right);
      }
    }

    assert(left_objects.size() < objects.size());
    assert(right_objects.size() < objects.size());

    if (!left_objects.empty())
      left = new Node(left_objects, left_boxes, left_bound, opt);

    if (!right_objects.empty())
      right = new Node(right_objects, right_boxes, right_bound, opt);
  }

  Node::~Node()
  {
    if (left)
      delete left;
    if (right)
      delete right;
  }

////////////////////////////////////////////////////////////////////////////////


  Node::split_plane::split_plane(scalar s, NodeAxis a) : split(s), axis(a)
  {
  }

  bool Node::split_plane::operator <(const Node::split_plane& rhs) const
  {
    return split < rhs.split;
  }

////////////////////////////////////////////////////////////////////////////////

  Tree::Tree(const vector<shared_ptr<Bounded>>& objects, const TreeOptions& opt)
  {
    auto boxes = vector<bounds::AABB>(objects.size());
    transform(objects.begin(), objects.end(), boxes.begin(),
              [](const auto& a) { return a->get_bounding_box(); });

    auto full_bound = accumulate(boxes.begin() + 1, boxes.end(), boxes[0], bounds::AABB::box_union);

    root = shared_ptr<Node>(new Node(objects, boxes, full_bound, opt));
  }
}
