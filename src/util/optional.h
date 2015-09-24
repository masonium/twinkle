#pragma once

#include <iostream>
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
using std::ostream;

struct optional_error : public logic_error
{
  optional_error(const string& what) : logic_error(what) { }
};

struct none_tag_t {};

extern none_tag_t none_tag;

/*
 * Basic optional type, for representing a value or "none".
 */
template <typename T>
class optional
{
public:
  optional() : _init(false) {}

  optional(none_tag_t) : _init(false) { }

  optional(const T& r) : _init(true) {
    memcpy(reinterpret_cast<void*>(_data), reinterpret_cast<const void*>(&r), sizeof(T));
  }

  bool is() const {
    return _init;
  }

  const T& get() const {
#ifndef NDEBUG
    if (!_init)
      throw optional_error("Optional value has no data.");
#endif
    return value();
  }

  const T& get(const T& default_value) const noexcept {
    return _init ? value() : default_value;
  }


  bool operator ==(const optional<T>& rhs)
  {
    if (_init != rhs._init)
      return false;
    return !_init || (value() == rhs._values);
  }

protected:
  // return the value,
  const T& value() const { return *reinterpret_cast<const T*>(_data); }
  bool init() const { return _init; }
  void clear() { _init = false; }

private:
  uint8_t _data[sizeof(T)];
  bool _init;
};

/**
 * forced_positive is like an optional type, but specifically forces its
 * contained value, when existent, to have a positive value. Any non-positive
 * values are treated as 'none' in the constructor.
 */
template <typename T, typename =  enable_if<std::is_arithmetic<T>::value> >
class forced_positive : public optional<T>
{
public:
  forced_positive() { }

  /*
   * The minor convenience of a non-explicit constructor is not worth the
   * trouble. The comparison operators also help with this.
   */
  explicit forced_positive(const T& r) : optional<T>(r)
  {
    if (r <= 0)
      this->clear();
  }

  /*
   * This operator is specifically designed to make finding the 'best'
   * intersection easier.
   */
  bool operator <(const forced_positive<T>& fp) const
  {
    if (!this->is())
      return false;

    return !fp.is() || (this->value() < fp.value());
  }

  bool operator <=(const forced_positive<T>& fp) const
  {
    if (!fp.is())
      return true;

    return this->is() && (this->value() <= fp.value());
  }

  /*
   * The semantics of comparisons with normal values is more subtle than I
   * initially thought. The obvious solution is to wrap the scalar as a
   * forced_positive and then perform the comparison. However, this is wrong in
   * the case of negative values, since they incorrectly get wrapped to none,
   * when they should actually just be compared as a negative.
   */
  bool operator <(const T& f) const
  {
    return this->is() && this->value() < f;
  }

  bool operator <=(const T& f) const
  {
    return this->is() && this->value() <= f;
  }
};

template <typename T>
bool operator <(const T& t, const forced_positive<T>& fp)
{
  return !fp.is() || (t < fp.get());
}

template <typename T>
bool operator <=(const T& t, const forced_positive<T>& fp)
{
  return !fp.is() || (t <= fp.get());
}

template <typename T>
ostream& operator <<(ostream& out, const forced_positive<T>& fp)
{
  if (fp.is())
    out << fp.get();
  else
    out << "NONE";

  return out;
}
