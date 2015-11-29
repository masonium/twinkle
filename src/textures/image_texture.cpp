#include "image_texture.h"
#include "util/images.h"
#include <cassert>

ImageTexture2D::ImageTexture2D(const Texture2D& tex, int width, int height)
  : _w(width), _h(height), _data(_w * _h)
{
  _resample_texture(tex);
}

ImageTexture2D::ImageTexture2D(const string& filename)
  : _w(0), _h(0)
{
  Image img;
  assert(load_image(filename, img));
  _w = img.width;
  _h = img.height;
  _data = std::move(img.data);
}

int ImageTexture2D::index(int x, int y) const
{
  return y * _w + x;
}

spectrum& ImageTexture2D::at(int x, int y)
{
  return _data[index(x, y)];
}
const spectrum& ImageTexture2D::at(int x, int y) const
{
  return _data[index(x, y)];
}

void ImageTexture2D::_resample_texture(const Texture2D& tex)
{
  for (auto y = 0u; y < _h; ++y)
  {
    scalar fy = (y + 0.5) / _h;
    for (auto x = 0u; x < _w; ++x)
    {
      Vec2 uv((x + 0.5) / _w, fy);
      at(x, y) = tex.at_coord(uv);
    }
  }
}

spectrum ImageTexture2D::at_coord(const Vec2& uv) const
{
  scalar u = fmod(uv[0], 1.0);
  if (u < 0)
    u += 1.0;
  scalar v = fmod(uv[1], 1.0);
  if (v < 0)
    v += 1.0;

  // nearest neighbor
  int x = u * _w;
  int y = v * _h;

  return at(x, y);
}
