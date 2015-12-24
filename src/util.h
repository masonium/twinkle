#pragma once

#include <unordered_map>
#include <iterator>
#include <string>

using std::iterator_traits;
using std::forward_iterator_tag;

uint num_system_procs();

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

/*
 */
template <typename T>
class Identity
{
public:
  Identity(const T& t) : val(t)
  {
  }

  template <typename... Args>
  T operator() (Args&&... args) const
  {
    return val;
  }

private:
  T val;
};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

std::string lowercase(const std::string&);

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif


#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x) x
#define unlikely(X) x
#endif
