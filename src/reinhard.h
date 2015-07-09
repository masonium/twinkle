#include "tonemap.h"

/**
 * Implements the global tone reproduction operator as specified in Eq. 4 of
 * Reinhard et. al's paper, "Photographic Tone Reproduction for Digital Images"
 */
class ReinhardGlobal : public ToneMapper
{
public:
  ReinhardGlobal(scalar mid = 0.18) : key_value(mid) {}

  void tonemap(const vector<spectrum>& input, vector<spectrum>& output,
               uint w, uint h) const override;

private:
  scalar key_value;
};

/**
 * Implements the local, automatic dodging-and-burning tone reproduction
 * operator as specified in Eq. 9 of Reinhard et. al's paper, "Photographic Tone
 * Reproduction for Digital Images"
 */
class ReinhardLocal : public ToneMapper
{
public:
  ReinhardLocal(scalar mid = 0.18, scalar center_surround = 1.6);

  void tonemap(const vector<spectrum>& input, vector<spectrum>& output,
               uint w, uint h) const override;

private:
  static vector<vector<scalar>> center_surround_functions(const vector<scalar>&,
                                                          uint, uint, int);

  static int gaussian_filter(scalar curr, vector<scalar>& filter);

  scalar key_value;
  scalar scale_param;
  scalar cs_ratio;
  int num_scales;
};
