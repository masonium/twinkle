#pragma once

#include "twinkle.h"
#include "sampler.h"
#include <stack>
#include <vector>

class PSSSampler : public Sampler
{
public:
  PSSSampler(int seed, scalar large_step_prob);

  using Sampler::sample_2d;
  scalar sample_1d(void) override;

  void sample_2d(scalar& r1, scalar& r2);
  void sample_4d(scalar& r1, scalar& r2, scalar& r3, scalar& r4) override;

  using Sampler::sample_5d;
  void sample_5d(scalar& r1, scalar& r2, scalar& r3, scalar& r4, scalar& r5) override;

private:
  struct Coord
  {
    scalar value;
    uint64_t modify_time;
  };

  using stack_entry = std::pair<uint64_t, Coord>;

  scalar lsp;
  bool large_step;
  std::vector<Coord> coords;

  std::stack<stack_entry> last_modified;
};
