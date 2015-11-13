#pragma once

#include <stack>
#include <vector>
#include "twinkle.h"
#include "sampler.h"
#include "misc_types.h"

class PrimarySpaceSampling
{
public:
  PrimarySpaceSampling(scalar total_intensity, scalar large_step_prob);

  class PSSampler;

  PSSampler nth_sampler(int index, int total);

  class PSSampler : public Sampler
  {
  private:
    PSSampler(PrimarySpaceSampling&, int index, int stride);

    PrimarySpaceSampling& pss;
    int initial_index, stride;
    int index;

  public:
    void new_sample();

    scalar sample_1d(void) override;

    using Sampler::sample_2d;
    void sample_2d(scalar& r1, scalar& r2) override;
    void sample_4d(scalar& r1, scalar& r2, scalar& r3, scalar& r4) override;

    using Sampler::sample_5d;
    void sample_5d(scalar& r1, scalar& r2, scalar& r3, scalar& r4, scalar& r5) override;

  private:
    scalar next_coord();
  };

private:
  /*
   * Update the element at coordinate i to the current global time.
   */
  scalar update_coord(int i);

  /*
   * perturb the scalar by a small value, for small-step changes
   */
  static scalar perturb_coord(scalar s, scalar u);

  using time_t = uint64_t;

  struct Sample
  {
    scalar w;
    Pixel location;
  };

  struct Coord
  {
    Coord() = default;
    Coord(scalar v, time_t t) :
      value(v), modify_time(t)
    {
    }

    scalar value;
    time_t modify_time;
  };

  using stack_entry = std::pair<uint64_t, Coord>;

  UniformSampler sampler;

  scalar lsp; // probability of a large step on a new sample
  scalar total_intensity;

  time_t global_time; // time of most recently-modified sample (also equivalent
                      // to total number of full samples taken.
  time_t large_step_time; // time since the last large step

  std::vector<Coord> coords;

  std::stack<stack_entry> last_modified;
  time_t index;
  int large_step;
  Sample old_sample;
};
