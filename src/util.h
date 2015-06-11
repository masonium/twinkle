#pragma once

#include <unordered_map>
#include <iterator>

using std::iterator_traits;
using std::forward_iterator_tag;

int num_system_procs();

template <typename T=int>
class iota_iterator
{
public:
  typedef T value_type;
  typedef forward_iterator_tag iterator_category;

  iota_iterator(T start = 0) : i(start) {}

  T operator *() const { return i; };
  iota_iterator<T>& operator++() { ++i; return *this; }
  iota_iterator<T> operator++(int i) { iota_iterator<T> x(*this); ++i; return x; }
  bool operator ==(const iota_iterator<T>& it) const { return it.i == i; }
  bool operator !=(const iota_iterator<T>& it) const { return it.i != i; }

private:
  T i;
};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}
