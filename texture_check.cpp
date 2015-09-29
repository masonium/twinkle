#include <random>
#include <lua.hpp>

#include "texture.h"
#include "perlin.h"
#include "film.h"
#include "tonemap.h"
#include "reinhard.h"
#include "textures/skytexture.h"

#define LUA_TEX

using std::make_shared;
using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char** args)
{
  assert(argc > 3);

  int width = atoi(args[1]);
  int height = atoi(args[2]);
  int angle = atoi(args[3]);

  auto filter = make_shared<BoxFilter>();
  Film f(width, height, filter);


#ifdef LUA_TEX
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  if (luaL_dofile(L, "test.lua"))
  {
    cerr << "error loading lua file" << endl;
    exit(1);
  }
#else
  auto check_tex = Checkerboard2D(spectrum{1.0, 0.0, 0.0}, spectrum{0.0, 0.0, 1.0}, 4);
#endif

  Ray r(Vec3::zero, Vec3::zero);

  scalar freq = 2;

  scalar min = 5, max = -5;
  scalar sw = width, sh = height;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      auto uv = Vec2{x / sw, y / sh};

#ifdef LUA_TEX
      lua_getglobal(L, "check");
      lua_pushnumber(L, uv[0]);
      lua_pushnumber(L, uv[1]);
      lua_call(L, 2, 3);

      auto s = spectrum(lua_tonumber(L, -1), lua_tonumber(L, -2), lua_tonumber(L, -3));
      lua_pop(L, 3);
#else
      auto s = check_tex.at_coord(uv);
#endif
      f.add_sample(PixelSample(x, y, 0.5, 0.5, r), s);

/*
      scalar p = unitize(sym_fbm_2d(pn, x / sw, y / sh, freq, 4, 3.0, 0.75));

      //scalar p = unitize(pn->noise(x / sw * freq, y / sh * freq));
      spectrum s;
      auto mapper = [](scalar x) { return x; };
      spectrum c2{0.2};
      if (p < 0.5)
        s = lerp(spectrum{0.8, 0.2, 0.2}, c2, mapper(p*2.0));
      else
        s = lerp(c2, spectrum{0.2, 0.1, 0.9}, mapper((p-0.5)*2.0));

      f.add_sample(PixelSample(x, y, 0.5, 0.5, r), s);//spectrum{static_cast<scalar>((p + 1.0) * 0.5)});
      if (min > p)
        min = p;
      if (max < p)
        max = p;
*/
      scalar p = s.luminance();
      if (min > p)
        min = p;
      if (max < p)
        max = p;
    }
  }

  cerr << min << ", " << max << endl;
  auto mapper = ReinhardLocal{ReinhardLocal::Options()};
  f.render_to_ppm(cout, mapper);
}
