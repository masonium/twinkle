#pragma once

#include <stack>
#include <vector>
#include "twinkle.h"
#include "sampler.h"
#include "film.h"

class PrimarySpaceSampling
{
private:
  class PSSampler;
  struct Sample;
  struct Coord;

public:
  struct Options
  {
    scalar large_step_prob;
  };

  PrimarySpaceSampling(const Options&, scalar ti);

  PSSampler nth_sampler(int index, int total);
  void seed(int);

  bool is_large_step() const { return large_step; }

  Sample finish_sample(const PixelSample&, const spectrum&);

private:
  using time_t = uint64_t;

  struct Sample
  {
    Sample() : weight(0), value{0.0}, location(0, 0, 0, 0, Ray(Vec3{0}, Vec3{1, 0, 0})) {}
    Sample(scalar w, spectrum v, PixelSample ps) : weight(w), value(v), location(ps) {}

    scalar weight;
    spectrum value;
    PixelSample location;
  };

  /*
   * Update the element at coordinate i to the current global time.
   */
  scalar update_coord(uint i);

  /*
   * perturb the scalar by a small value, for small-step changes
   */
  static scalar perturb_coord(scalar s, scalar u);

  using stack_entry = std::pair<uint64_t, Coord>;

  UniformSampler sampler;

  Options options;
  scalar total_intensity;

  time_t global_time; // time of most recently-modified sample (also equivalent
                      // to total number of full samples taken.
  time_t large_step_time; // time since the last large step
  scalar old_intensity;

  std::vector<Coord> coords;
  std::vector<stack_entry> last_modified;

  int large_step;
  Sample old_sample;

  struct Coord
  {
    Coord() : value(0), modify_time(0) { }
    Coord(scalar v, time_t t) :
      value(v), modify_time(t)
    {
    }

    scalar value;
    time_t modify_time;
  };

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
    friend class PrimarySpaceSampling;
    scalar next_coord();
  };
};

using PSSMLT = PrimarySpaceSampling;
