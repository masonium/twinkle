#include "perlin.h"
#include <algorithm>
#include <random>

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
    scalar th = unf(eng) * 2 * PI;
    grad_2d[i] = Vec2(cos(th), sin(th));
    grad_1d[i] = unf(eng) * 2 - 1;
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

scalar PerlinNoise::noise(scalar x) const
{
  int X = static_cast<int>(x);
  scalar dx = x - X;

  scalar u = p5_interp(dx);

  auto px0 = perm_x[X % 256], px1 = perm_x[(X+1) % 256];

  auto g0 = grad_1d[px0];
  auto g1 = grad_1d[px1];

  return lerp( g0 * dx, g1 * dx-1, u);
}

scalar PerlinNoise::noise(scalar x, scalar y) const
{
  int X = static_cast<int>(x), Y = static_cast<int>(y);
  scalar dx = x - X;
  scalar dy = y - Y;

  scalar u = p5_interp(dx), v = p5_interp(dy);

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

scalar PerlinNoise::noise(scalar x, scalar y, scalar z) const
{
  int X = static_cast<int>(x), Y = static_cast<int>(y), Z = static_cast<int>(z);
  scalar dx = x - X;
  scalar dy = y - Y;
  scalar dz = z - Z;

  scalar u = p5_interp(dx), v = p5_interp(dy), w = p5_interp(dz);

  auto px0 = perm_x[X % 256] % 16, px1 = perm_x[(X+1) % 256] % 16;
  auto py0 = perm_y[Y % 256] % 16, py1 = perm_y[(Y+1) % 256] % 16;
  auto pz0 = perm_z[Z % 256] % 16, pz1 = perm_z[(Z+1) % 256] % 16;

  auto& g000 = grad_3d[(px0 ^ py0 ^ pz0)];
  auto& g010 = grad_3d[(px0 ^ py1 ^ pz0)];
  auto& g100 = grad_3d[(px1 ^ py0 ^ pz0)];
  auto& g110 = grad_3d[(px1 ^ py1 ^ pz0)];
  auto& g001 = grad_3d[(px0 ^ py0 ^ pz1)];
  auto& g011 = grad_3d[(px0 ^ py1 ^ pz1)];
  auto& g101 = grad_3d[(px1 ^ py0 ^ pz1)];
  auto& g111 = grad_3d[(px1 ^ py1 ^ pz1)];

  return lerp( lerp(lerp( vdot(g000, dx, dy, dz), vdot(g100, dx-1, dy, dz), u),
                    lerp( vdot(g010, dx, dy-1, dz), vdot(g110, dx-1, dy-1, dz), u),
                    v),
               lerp(lerp( vdot(g001, dx, dy, dz-1), vdot(g101, dx-1, dy, dz-1), u),
                    lerp( vdot(g011, dx, dy-1, dz-1), vdot(g111, dx-1, dy-1, dz-1), u),
                    v),
               w);
}

scalar sym_fbm_1d(shared_ptr<PerlinNoise> noise, scalar x,
               scalar freq, scalar octaves,
               scalar lacunarity, scalar persistence)
{
  scalar sum = 0;
  scalar amplitude = 1;
  octaves -= 1;
  while (octaves > 0)
  {
    sum += noise->noise(x * freq) * amplitude * std::min<scalar>(octaves, 1.0);
    amplitude *= persistence;
    freq *= lacunarity;
    octaves -= 1;
  }
  return sum;
}

scalar sym_fbm_2d(shared_ptr<PerlinNoise> noise,
               scalar x, scalar y,
               scalar freq, scalar octaves,
               scalar lacunarity, scalar persistence)
{
  scalar sum = 0;
  scalar amplitude = 1;
  octaves -= 1;
  while (octaves > 0)
  {
    sum += noise->noise(x * freq, y * freq) * amplitude * std::min<scalar>(octaves, 1.0);
    amplitude *= persistence;
    freq *= lacunarity;
    octaves -= 1;
  }
  return sum;
}
