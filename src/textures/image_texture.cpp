#include "textures/image_texture.h"
#include <cassert>

ImageTexture2D::ImageTexture2D(const Texture2D& tex, size_t width, size_t height)
  : _image(width, height)
{
  _resample_texture(tex);
}

ImageTexture2D::ImageTexture2D(const string& filename)
{
  _image = load_image(filename);
  assert(_image.width() > 0);
}

spectrum& ImageTexture2D::at(size_t x, size_t y)
{
  return _image(x, y);
}
const spectrum& ImageTexture2D::at(size_t x, size_t y) const
{
  return _image(x, y);
}

void ImageTexture2D::_resample_texture(const Texture2D& tex)
{
  for (auto y = 0u; y < _image.height(); ++y)
  {
    scalar fy = (y + 0.5) / _image.height();
    for (auto x = 0u; x < _image.width(); ++x)
    {
      Vec2 uv((x + 0.5) / _image.width(), fy);
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
  int x = u * _image.width();
  int y = v * _image.height();

  return at(x, y);
}
