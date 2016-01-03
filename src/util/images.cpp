#include "images.h"
#include "util/filesystem.h"
#include <iostream>


using std::cerr;
using std::endl;

void ascii_image(ostream& out, const sp_image& image)
{
  for (int y = image.height() - 1; y >= 0; --y)
  {
    for (uint x = 0; x < image.width(); ++x)
    {
      auto s = image(x,y);
      if (norm(s) > 0.5)
        out << '*';
      else if (norm(s) > 0.25)
        out << '.';
      else
        out << ' ';
    }
    out << '\n';
  }
}

#if FEATURE_IMAGE_LOADING
#include <Magick++.h>
using Magick::Color;

Color to_magick(const spectrum& c)
{
  scalar s = ((1 << QuantumDepth) - 1);

  Color color;
  color.redQuantum(c[0] * s);
  color.greenQuantum(c[1] * s);
  color.blueQuantum(c[2] * s);
  return color;
}

spectrum from_magick(const Color& c)
{
  scalar m = 1.0 / ((1 << QuantumDepth) - 1);
  auto a =  spectrum{c.redQuantum() * m, c.greenQuantum() * m, c.blueQuantum() * m};
  //cerr << a << endl;
  return a;
}

sp_image load_image(const std::string& filename)
{
  sp_image image;

  if (!filesystem::exists(filename))
  {
    cerr << "Could not find image file: " << filename << endl;
    return image;
  }

  Magick::Image file_img(filename);

  if (file_img.columns() == 0 || file_img.rows() == 0)
  {
    cerr << "Could not load image: " << filename << endl;
    return image;
  }
  cerr << "Loaded a " << file_img.columns() << "x" << file_img.rows() << " image." << endl;

  image.resize(file_img.columns(), file_img.rows());

  for (uint ry = 0; ry < image.height(); ++ry)
  {
    uint iy = image.height() - 1 - ry;
    for (uint x = 0; x < image.width(); ++x)
    {
      image(x, iy) = from_magick(file_img.pixelColor(x, ry));
    }
  }

  return image;
}

bool save_image(const std::string& filename, const sp_image& image)
{
  if (image.width() == 0 || image.height() == 0)
  {
    cerr << "Cannot save an empty image.\n";
    return false;
  }

  cerr << "Saving image of size " << image.width() << "x" << image.height() << endl;

  Magick::Geometry image_geom(image.width(), image.height());
  Magick::Image file_img(image_geom, Magick::Color(0, 0, 0));

  file_img.modifyImage();
  file_img.type(Magick::TrueColorType);

  auto packet = file_img.getPixels(0, 0, image.width(), image.height());

  uint64_t pi = 0;
  for (int ry = image.height() - 1; ry >= 0; --ry)
  {
    for (uint x = 0; x < image.width(); ++x, ++pi)
    {
      packet[pi] = to_magick(image(x, ry));
    }
  }

  file_img.syncPixels();
  file_img.write(filename);
  return true;
}

#else

bool save_image_ppm(const std::string& filename, const sp_image& image)
{
  if (image.width() == 0 || image.height() == 0)
  {
    cerr << "Cannot save an empty image.\n";
    return false;
  }

  ofstream out(filename);
  out << "P3 " << width << " " << height << " 255\n";

  for (int y = image.height() - 1; y >= 0; --y)
  {
    for(uint x = 0; x < image.width(); ++x)
    {
      const auto& c = final(x, y);//.clamp(0, 1);
      if (std::isnan(c.x) || c.x < 0 || c.y < 0 || c.z < 0)
      {
        cerr << x << ", " << y << ", " << c << std::endl;
        const auto& p = plate[index(x, y)];
        cerr << p.mean << ", " << p.ss << ", " << p.weight << std::endl;
      }

      assert(c.x >= 0);
      assert(c.y >= 0);
      assert(c.z >= 0);

      assert(c.x <= 1);
      assert(c.y <= 1);
      assert(c.z <= 1);
      out << int(c.x * 255) << " " << int(c.y * 255) << " " << int(c.z * 255) << " ";
    }
    out << "\n";
  }

  return true;
}

sp_image load_image(const std::string& filename)
{
  cerr << "Compile with FEATURE_IMAGE_LOADING=1 to turn on image file loading.\n";
  assert(false);
  return sp_image();
}

#endif
