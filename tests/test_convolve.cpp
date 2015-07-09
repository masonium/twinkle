#include "math_util.h"
#include "test_util.h"

void conv_test_set(vector<scalar>& data, vector<scalar>& filter, vector<scalar>& expected_conv)
{
  data.resize(25);
  for (int i = 0; i < 25; ++i) data[i] = i;

  filter = {0, 1, 0, 1, 2, 1, 0, 1, 0};

  expected_conv = {6, 10, 15, 20, 20,
                   26, 36, 42, 48, 44,
                   51, 66, 72, 78, 69,
                   76, 96, 102, 108, 94,
                   76, 100, 105, 110, 90};
}

SUITE(convolution)
{
  TEST(naive_conv1)
  {
    vector<scalar> data, filter, conv, expected_conv;
    conv_test_set(data, filter, expected_conv);
    convolve(5, 5, data, 3, 3, filter, expected_conv);

    CHECK_ARRAY(conv.data(), expected_conv.data(), 25);
  }
}
