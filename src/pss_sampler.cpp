#include "pss_sampler.h"

using PSS = PrimarySpaceSampling;

PSS::PrimarySpaceSampling(scalar t, scalar large_step_prob)
  : lsp(large_step_prob), total_intensity(t)
{
  coords.resize(100);
  fill(begin(coords), end(coords), Coord(0, 0));
}

void PSS::seed(int s)
{
  sampler.seed(s);
}

void PSS::new_sample()
{
  index = 0;
}

void PSS::finish_sample(scalar intensity, Pixel p)
{
  scalar accept_prob = old_intesity > 0 ? min<scalar>(1.0, intensity / old_intensity) : 1.0;

  // In the original paper, the weight is divided by M, the total expected
  // number of samples. This is because we add all the samples together, rather
  // than averaging them. Since we're going to average the samples together, we
  // shouldn't include that term.
  scalar w = (a + large_step) / (intensity / total_intensity + lsp);
  Sample new_sample{w, p};

  old_sample.weight += (1-a) / (old_intensity / total_intensity + lsp);

  Sample contrib_sample;
  // If we accept the new sample, return the old one.
  if (sampler.sample_1d() < accept_prob)
  {
    old_intensity = intensity;

    contrib_sample = old_sample;
    old_sample = new_sample;
    if (large_step)
      large_step_time = global_time;
    ++global_time;
    last_modified.clear();
  }
}

scalar PSS::update_coord(int index)
{
  if (index >= coords.size())
  {
    scalar u = sampler.sample_1d();
    coords.resize(index+1);
    coords.emplace_back(u, global_time);
  }

  auto& coord = coords[index];

  if (coord.modify_time < global_time)
  {
    /*
     * For a large step, we use this as the new time we had a value worth
     * remembering.
     */
    if (large_step)
    {
      last_modified.emplace(index, coord);
      coord.value = sampler.sample_1d();
      coord.modify_time = large_step_time;
    }
    /*
     * Small step.
     */
    else
    {
      /*
       * If we haven't used the coordinate since the most recent large step,
       * just skip to that point.
       */
      if (coord.modify_time < large_step_time)
      {
        coord.value = sampler.sample_1d();
        coord.modify_time = large_step_time;
      }

      /*
       * Once we know we're past a large time step, update with small
       * perturbations until we are at the current time.
       */
      const auto num_mutations = global_time - coord.modify_time;
      for (auto i = 0u; i < num_mutations; ++i)
      {
        coord.value = perturb_coord(coord.value, sampler.sample_1d());
      }
      coord.modify_time = global_time;
    }
  }

  return coord.value;
}

scalar PSSampler::perturb_coord(scalar s, scalar u)
{
  const scalar s1 = 1.0/1024, s2 = 1.0/64;
  const scalar lm = -log(s2 / s1);
  auto v = s +s2 * exp(lm * u);
  return v > 1 ? v - 1 : (v < 0 ? v + 1 : v);
}

////////////////////////////////////////////////////////////////////////////////

void PrimarySpaceSampling::PSSampler::new_sample()
{
  index = initial_index;
}

scalar PrimarySpaceSampling::PSSampler::next_coord()
{
  scalar s = pss.get_coord(index);
  index += stride;
  return s;
}

scalar PrimarySpaceSampling::PSSampler::sample_1d(void)
{
  return next_coord();
}

void PrimarySpaceSampling::PSSampler::sample_2d(scalar& r1, scalar& r2)
{
  r1 = next_coord();
  r2 = next_coord();
}

void PrimarySpaceSampling::PSSampler::sample_4d(scalar& r1, scalar& r2, scalar& r3,
                                                scalar& r4)
{
  sample_2d(r1, r2);
  sample_2d(r3, r4);
}

void PrimarySpaceSampling::PSSampler::sample_5d(scalar& r1, scalar& r2, scalar& r3,
                                                scalar& r4, scalar& r5)
{
  sample_4d(r1, r2, r3, r4);
  r5 = sample_1d();
}
