#include "texture.h"
#include "perlin.h"
#include "film.h"
#include "tonemap.h"
#include "reinhard.h"
#include "textures/skytexture.h"
#include "textures/hosekskytexture.h"
#include "cpp-optparse/OptionParser.h"
#include <random>

using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::cerr;
using std::cout;
using std::endl;

class NoiseTexture : public Texture2D
{
public:
  NoiseTexture() : pn(PerlinNoise::get_instance())
  {

  }

  spectrum at_coord(const Vec2& uv) const
  {
    auto s = sym_fbm_2d(pn, uv.u, uv.v, 4.0, 5.0);
    if (s < 0)
      return lerp(spectrum{1.0, 0.0, 0.0}, spectrum{0.0}, sqrt(s + 1));
    else
      return lerp(spectrum{0.0}, spectrum{0.0, 0.0, 1.0}, sqrt(s));
    //return spectrum((s+1.0)*0.5);
  }
private:
  shared_ptr<PerlinNoise> pn;
};

int main(int argc, char** args)
{

  optparse::OptionParser parser{};
  parser.usage("generate textures");
  parser.version("0.0.0");

  vector<string> texture_choices({"noise", "sky", "hoseksky"});
  vector<string> mapper_choices({"linear", "rh_global", "rh_local"});

  parser.add_option("-w", "--width").action("store").type("int").set_default("400");
  parser.add_option("-h", "--height").action("store").type("int").set_default("300");
  parser.add_option("--type").action("store").choices(texture_choices).set_default("hosek_sky");
  parser.add_option("--mapper").action("store").choices(mapper_choices).set_default("rh_global");
  parser.add_option("-a", "--angle").action("store").type("float").set_default("40");
  parser.add_option("-t", "--turbidity").action("store").type("float").set_default("3.0")
    .help("turbidity of the sky models");
  parser.add_option("-o", "--output_file").action("store");

  auto& options = parser.parse_args(argc, args);

  int width = options.get("width").as<int>();
  int height = options.get("height").as<int>();
  scalar angle = options.get("angle").as<scalar>();

  Film f(width, height);

  unique_ptr<Texture2D> tex;
  string tex_type = options.get("type");
  if (tex_type == "noise")
  {
    cerr << "making noise";
    tex = make_unique<NoiseTexture>();
  }
  else if (tex_type == "sky")
  {
    double turbidity = options.get("turbidity").as<double>();
    tex = make_unique<ShirleySkyTexture>(Vec3::from_euler(3*PI/4, angle * PI / 180), turbidity);
  }
  else // if (tex_type == "hoseksky")
  {
    double turbidity = options.get("turbidity").as<double>();
    tex = make_unique<HosekSkyTexture>(Vec3::from_euler(3*PI/4, angle * PI / 180), turbidity, false);
  }

  Ray r(Vec3::zero, Vec3::zero);

  scalar min = 5, max = -5;
  scalar sw = width, sh = height;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      auto uv = Vec2(x / sw, (y / sh));

      auto s = tex->at_coord(uv);
      if (s.isnan())
        cerr << x << ", " << y << ": " << s << "\n" << std::flush;
      assert(!s.isnan());
      f.add_sample(PixelSample(x, y, 0.5, 0.5, r), s);

      scalar p = s.luminance();
      if (min > p)
        min = p;
      if (max < p)
        max = p;
    }
  }
  cerr << min << ", " << max << endl;

  shared_ptr<ToneMapper> mapper;
  if (options.get("mapper") == "linear")
    mapper = make_shared<LinearToneMapper>();
  else if (options.get("mapper") == "rh_global")
    mapper = make_shared<ReinhardGlobal>();
  else
    mapper = make_shared<ReinhardLocal>(ReinhardLocal::Options());

  string filename = options.get("output_file");
  save_image(filename, mapper->tonemap(f.image()));
}
