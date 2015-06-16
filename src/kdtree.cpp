#include "kdtree.h"
#include <algorithm>

using std::max_element;
using std::distance;
using std::count_if;
using std::tie;
using std::move;

namespace kd
{

  Node::Node(const vector<shared_ptr<Bounded>>& objects,
             const vector<bounds::AABB>& boxes,
             const bounds::AABB& total_bound, const TreeOptions& opt) : bound(total_bound)
  {
    auto bb_size = total_bound.size();
    int longest_axis = distance(bb_size.v,max_element(bb_size.v, bb_size.v + 3));

    const scalar sa = total_bound.surface_area();

    int num_samples = opt.num_uniform_samples + opt.num_adaptive_samples;
    vector<split_eval> split_evals(num_samples);

    const auto num_boxes = boxes.size();

    for (int ai = 0; ai < 3; ++ai)
    {
      if (bb_size[ai] == 0)
        continue;

      if (num_boxes <= opt.hybrid_one_axis_limit && ai != longest_axis)
        continue;

      auto axis = static_cast<NodeAxis>(ai);

      /*
       * 1. Uniform sampling.
       *
       * Sample uniformly along the axis.
       */
      for (auto i = 0u; i < opt.num_uniform_samples; ++i)
      {

        auto sp = split_plane{total_bound.min[ai] + bb_size[ai] * i / (opt.num_uniform_samples - 1), axis};

        split_evals[i] = evaluate_split(sp, boxes, opt, sa);
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
      for (auto i = 0u; i < opt.num_adaptive_samples - 1; ++i)
      {
        scalar f = scalar(i+1) / opt.num_adaptive_samples;
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
      for (int i = num_inner_samples.size() - 1, ei = num_samples - 1; i >= 0; ++i)
      {
        int end_idx = ei--;
        for (int j = num_inner_samples[i] - 1; j >= 0; --j, --ei)
        {
          scalar f = (j+1) / (num_inner_samples[i] + 1);
          scalar split = split_evals[i].split * (1-f) + split_evals[i+1].split * f;
          split_evals[ei] = evaluate_split(split_plane{split, axis}, boxes, opt, sa);
        }
        split_evals[end_idx] = move(split_evals[i+1]);
      }
    }
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
                            }) + num_middle;
    int num_right = boxes.size() - num_left + num_middle;

    scalar sa_left = 0, sa_right = 0;
    tie(sa_left, sa_right) = child_areas(bound, sp);

    scalar cost = opt.self_traversal_cost + (num_left * sa_left + num_right * sa_right) / surface_area;
    if (num_left == 0 || num_right == 0)
      cost *= opt.empty_side_discount;

    return split_eval{sp.split, cost, static_cast<scalar>(num_right - num_left)};
  }

  Node::split_plane::split_plane(scalar s, NodeAxis a) : split(s), axis(a)
  {
  }

  Node::~Node()
  {
    if (left)
      delete left;
    if (right)
      delete right;
  }
}
