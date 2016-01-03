#pragma once

#include "texture.h"
#include "util/images.h"
#include <vector>

using std::vector;

class ImageTexture2D : public Texture2D
{
public:
  ImageTexture2D(const std::string& filename);
  ImageTexture2D(const Texture2D& tex, size_t width, size_t height);

  uint width() const;
  uint height() const;

  spectrum at_coord(const Vec2& uv) const override;

private:
  spectrum& at(size_t x, size_t y);
  const spectrum& at(size_t x, size_t y) const;

  spectrum& operator ()(int x, int y) { return _image(x, y); }

  void _resample_texture(const Texture2D& tex);

  sp_image _image;
};
