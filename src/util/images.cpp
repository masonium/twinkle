#include "images.h"
#include "util/filesystem.h"
#include <Magick++.h>
#include <iostream>

using Magick::Color;
using std::cerr;
using std::endl;

#if FEATURE_IMAGE_LOADING

Color to_magick(const spectrum& c)
{
  scalar s = ((1 << QuantumDepth) - 1);
  spectrum clamped = c.clamp(0, 1);

  Color color;
  color.redQuantum(clamped[0] * s);
  color.greenQuantum(clamped[1] * s);
  color.blueQuantum(clamped[2] * s);
  return color;
}

spectrum from_magick(const Color& c)
{
  scalar m = 1.0 / ((1 << QuantumDepth) - 1);
  return spectrum{c.redQuantum() * m, c.greenQuantum() * m, c.blueQuantum() * m};
}

bool load_image(const std::string& filename, Image& image)
{
  if (!filesystem::exists(filename))
  {
    cerr << "Could not find image file: " << filename << endl;
    return false;
  }

  Magick::Image img(filename);

  image.width = img.columns();
  image.height = img.rows();

  if (image.width == 0 || image.height == 0)
  {
    cerr << "Could not load image: " << filename << endl;
    return false;
  }

  image.data.resize(image.width * image.height);

  int pi = 0;
  for (int ry = 0; ry < image.height; ++ry)
  {
    for (int x = 0; x < image.width; ++x, ++pi)
    {
      image.data[pi] = from_magick(img.pixelColor(x, ry));
    }
  }

  return true;
}

#else

bool load_image(const std::string& filename, Image& image)
{
  cerr << "Compile with FEATURE_IMAGE_LOADING=1 to turn on image file loading.\n";
  return false;
}

#endif
