#include "tonemap.h"

using std::pair;

/**
 * Implements the global tone reproduction operator as specified in Eq. 4 of
 * Reinhard et. al's paper, "Photographic Tone Reproduction for Digital Images"
 */
class ReinhardGlobal : public ToneMapper
{
public:
  ReinhardGlobal(scalar mid = 0.18) : key_value(mid) {}

  sp_image tonemap(const sp_image& input) const override;

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
  struct Options
  {
    Options();

    scalar key_value; // a, in the paper
    scalar scale_selection_threshhold; // epsilon, in the paper
    scalar sharpening_factor;  // \phi, in the paper
    scalar scale_pixel_factor; // \alpha_1, in the paper
    scalar cs_ratio;
    int num_scales;
  };


  ReinhardLocal(const Options&);

  sp_image tonemap(const sp_image& input) const override;

private:
  static vector<vector<pair<scalar, scalar>>> center_surround_functions(uint, uint, const vector<scalar>&, const Options&);

  static int gaussian_filter(scalar curr, vector<scalar>& filter);
  Options opt;
};
