#include "tonemap.h"

class ReinhardLocal : public ToneMapper
{
public:
  ReinhardLocal(scalar mid = 0.18) : middle_luminance(mid) {}

  void tonemap(const vector<spectrum>& input, vector<spectrum>& output,
               uint w, uint h) const override;

private:
  scalar middle_luminance;
};



class ReinhardGlobal : public ToneMapper
{
public:
  ReinhardGlobal(scalar mid = 0.18) : middle_luminance(mid) {}

  void tonemap(const vector<spectrum>& input, vector<spectrum>& output,
               uint w, uint h) const override;

private:
  scalar middle_luminance;
};
