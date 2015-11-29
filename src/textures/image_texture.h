#pragma once

#include "texture.h"
#include <vector>

using std::vector;

class ImageTexture2D : public Texture2D
{
public:
  ImageTexture2D(const std::string& filename);
  ImageTexture2D(const Texture2D& tex, int width, int height);

  uint width() const;
  uint height() const;

  spectrum at_coord(const Vec2& uv) const override;

private:
  int index(int x, int y) const;
  spectrum& at(int x, int y);
  const spectrum& at(int x, int y) const;

  spectrum& operator ()(int x, int y) { return at(x, y); }

  void _resample_texture(const Texture2D& tex);

  uint _w, _h;
  vector<spectrum> _data;
};
