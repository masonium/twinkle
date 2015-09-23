#pragma once

#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <cstring>
#include <cassert>
#include <type_traits>

using std::string;
using std::logic_error;
using std::enable_if;

struct optional_error : public logic_error
{
  optional_error(const string& what) : logic_error(what) { }
};

/*
 *
 */
template <typename T, typename =  enable_if<std::is_pod<T>::value> >
class optional
{
public:
  optional() : _init(false) {}

  optional(const T& r) :  _value(r), _init(true) {
  }

  operator bool() const { return _init; }

  const T& get() const {
#ifndef NDEBUG
    if (!_init)
      throw optional_error("Optional value has no data.");
#endif
    return _value;
  }

  const T& get(const T& default_value) const noexcept {
    return _init ? _value : default_value;
  }

private:
  T _value;
  bool _init;
};

template <typename T>
class forced_positive : public optional<T>
{
public:
  forced_positive() { }

  forced_positive(const T& r) : optional<T>(r) {
    assert(r > 0);
  }
  static forced_positive<T> none;
};

template <typename T>
forced_positive<T> forced_positive<T>::none;

