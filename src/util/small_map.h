#pragma once

#include <vector>
#include <cstdint>
#include <cassert>

using std::vector;

template <int N, typename K, typename V>
class SmallMap
{
public:
  SmallMap() : size(0) { }

  using key_type = K;
  using value_type = V;

  const value_type& operator[](const K& key) const
  {
    int i = find(key);
    assert(i >= 0);
    return _values[i];
  }
  value_type& operator[](const K& key)
  {
    int i = find(key);
    assert(i >= 0);
    return _values[i];
  }

  using iterator = V*;
  using const_iterator = const V*;

  iterator begin()
  {
    return _values;
  }
  iterator end()
  {
    return _values + size;
  }

  const_iterator begin() const
  {
    return _values;
  }

  const_iterator end() const
  {
    return _values + size;
  }
  void insert(K&& key, V&& value)
  {
    int idx = find(key);
    if (idx >= 0)
      _values[idx] = value;
    else
    {
      _keys[size] = key;
      _values[size] = value;
      ++size;
    }
    assert(size <= N);
  }

  void emplace(K&& key, V&& value)
  {
    int idx = find(key);
    if (idx >= 0)
      _values[idx] = std::move(value);
    else
    {
      _keys[size] = key;
      _values[size] = std::move(value);
      ++size;
    }
    assert(size <= N);
  }


private:
  K _keys[N];
  V _values[N];

  uint8_t size;

  int find(const K& key) const
  {
    for (uint8_t i = 0u; i < size; ++i)
      if (_keys[i] == key)
        return i;

    return -1;
  }

};
