#include "texture.h"
#include "film.h"
#include "tonemap.h"
#include "guile/types.h"

void inner_main(void* data, int argc, char** args)
{
  init_vec3();

  assert(argc > 2);

  int width = atoi(args[1]);
  int height = atoi(args[2]);

  ifstream runfile("noise.scm");
  string s = runfile.rdbuf();

  init_vec3();
  svm_eval_string(s.c_str());

  // auto filter = make_shared<BoxFilter>();
  // Film f(width, height, filter.get());

  // Ray r(Vec3::zero, Vec3::zero);

  // for (int y = 0; y < height; ++y)
  // {
  //   for (int x = 0; x < width; ++x)
  //   {
  //     PixelSample ps(x, y, 0.5, 0.5, r);

  //     f.add_sample(ps, s);
  //     if (min > p)
  //       min = p;
  //     if (max < p)
  //       max = p;
  //   }
  // }
  // cerr << min << ", " << max << endl;
  // f.render_to_ppm(cout, make_shared<CutoffToneMapper>());
}

int main(int argc, char** args)
{
  scm_boot_guile(argc, args, inner_main, NULL);
}
