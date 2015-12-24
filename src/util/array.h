#pragma once
#include <unistd.h>
#include <vector>

using std::vector;

template <typename T>
class Array2D
{
public:
  using value_type = T;

  Array2D(uint w, uint h) : _w(w), _data(w*h)
  {
  }

  template <typename container_type>
  Array2D(uint w, uint h, container_type&& data) :
    _w(w), _data(data)
  {
    assert(_data.size() == w * h);
  }

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
