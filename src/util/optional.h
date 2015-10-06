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
template <typename T, typename = typename enable_if<std::is_trivially_destructible<T>::value >::type >
class optional
{
public:
  optional(none_tag_t) : _init(false) { }

  template <typename... Args>
  optional(Args&&... args) : _init(true)
  {
    new(&_val) T(std::forward<Args>(args)...);
  }

  bool is() const
  {
    return _init;
  }

  const T& get() const
  {
    assert(_init);
    return value();
  }

  const T& get(const T& default_value) const noexcept
  {
    return _init ? value() : default_value;
  }


  bool operator ==(const optional<T>& rhs)
  {
    if (_init != rhs._init)
      return false;
    return !_init || (value() == rhs._values);
  }

protected:
  const T& value() const { return *reinterpret_cast<const T*>(&_val); }
  bool init() const { return _init; }
  void clear() { _init = false; }

private:
  using raw = typename std::aligned_storage<sizeof(T)>::type;
  raw _val;
  bool _init;
};

/**
 * forced_positive is like an optional type, but specifically forces its
 * contained value, when existent, to have a positive value. Any non-positive
 * values are treated as 'none' in the constructor.
 */
template <typename T, typename =  typename enable_if<std::is_arithmetic<T>::value>::type >
class forced_positive : public optional<T>
{
public:
  forced_positive(none_tag_t nt) : optional<T>{nt} { }

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
