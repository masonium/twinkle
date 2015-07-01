#include "perlin.h"
#include <algorithm>

using std::cerr;
using std::make_shared;
using std::copy;
using std::random_shuffle;

static const scalar INVSQRT2 = 1.0 / sqrt(2);
static const scalar IS2 = 1.0 / sqrt(2);

PerlinNoise::PerlinNoise() :
  grad_3d{{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1},{1,1,0},{0,-1,1},{-1,1,0},{0,-1,-1}}
{
  auto eng = std::mt19937();
  auto unf = std::uniform_real_distribution<scalar>(0, 256);

  for (int i = 0; i < 256; ++i)
  {
    perm_x[i] = i;
    scalar th = unf(eng);
    grad_2d[i] = Vec2(cos(th), sin(th));
  }

  copy(perm_x, perm_x + 256, perm_y);
  copy(perm_x, perm_x + 256, perm_z);

  random_shuffle(perm_x, perm_x + 256);
  random_shuffle(perm_y, perm_y + 256);
  random_shuffle(perm_z, perm_z + 256);
}

shared_ptr<PerlinNoise> PerlinNoise::get_instance()
{
  static auto instance = make_shared<PerlinNoise>();
  return instance;
}


scalar PerlinNoise::noise(scalar x, scalar y) const
{
  int X = static_cast<int>(x), Y = static_cast<int>(y);
  scalar dx = x - X;
  scalar dy = y - Y;

  scalar u = scurve(dx), v = scurve(dy);

  auto px0 = perm_x[X % 256], px1 = perm_x[(X+1) % 256];
  auto py0 = perm_y[Y % 256], py1 = perm_y[(Y+1) % 256];

  //cerr << (px0 ^ py0) << ", " << (px1 ^ py0) << ", " << (px0 ^ py1) << ", " << (px1 ^ py1) << "; ";
  auto g00 = grad_2d[(px0 ^ py0)];
  auto g01 = grad_2d[(px0 ^ py1)];
  auto g10 = grad_2d[(px1 ^ py0)];
  auto g11 = grad_2d[(px1 ^ py1)];

  return lerp(lerp( vdot(g00, dx, dy), vdot(g10, dx-1, dy), u),
              lerp( vdot(g01, dx, dy-1), vdot(g11, dx-1, dy-1), u),
              v);
}

scalar PerlinNoise::scurve(scalar t)
{
  return ((6.0 * t - 15.0) * t + 10.0) * t * t * t;
}
