#pragma once
#include <unistd.h>
#include <vector>

using std::vector;

template <typename T>
class Array2D
{
public:
  using value_type = T;

  Array2D(size_t w, size_t h) : _w(w), _data(w*h)
  {
  }

  template <typename container_type>
  Array2D(size_t w, size_t h, container_type&& data) :
    _w(w), _data(data)
  {
    assert(_data.size() == w * h);
  }

  void resize(size_t new_w, size_t new_h)
  {
    // Special case: If the widths are the same, we don't need to copy any data.
    if (_w == new_w)
    {
      _data.resize(new_w * new_h);
      _w = new_w;
      return;
    }

    // Otherwise, copy the data that will exist in the new version
    const size_t mw = std::min(_w, new_w);
    const size_t mh = std::min(height(), new_h);

    vector<T> new_data(new_w * new_h);
    for (int y = 0; y < mh; ++y)
    {
      copy(_data.data() + y * _w, _data.data() + y * _w + mw,
           new_data.data() + y * new_w);
    }

    swap(_data, new_data);
    _w = new_w;
  }

  size_t width() const
  {
    return _w;
  }

  size_t height() const
  {
    return _data.size() / _w;
  }

  size_t size() const
  {
    return _data.size();
  }

  auto begin() { return _data.begin(); }
  auto end() { return _data.end(); }

  auto begin() const { return _data.begin(); }
  auto end() const { return _data.end(); }

  T& operator()(int i, int j)
  {
    return _data[index(i, j)];
  }

  const T& operator()(int i, int j) const
  {
    return _data[index(i, j)];
  }

private:
  int index(int i, int j) const
  {
    return j * _w + i;
  }

  size_t _w;
  std::vector<T> _data;
};
