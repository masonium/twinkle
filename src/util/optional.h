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

  bool is() const {
    return _init;
  }

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


  bool operator ==(const optional<T>& rhs)
  {
    if (_init != rhs._init)
      return false;
    return !_init || (_value == rhs._values);
  }

protected:
  void clear() { _init = false; }

  T _value;
  bool _init;
};

template <typename T>
class forced_positive : public optional<T>
{
public:
  forced_positive() { }

  explicit forced_positive(const T& r) : optional<T>(r)
  {
#ifndef NDEBUG
    if (r <= 0)
      this->clear();
#endif
  }

  /*
   * This operator is specifically designed to make finding the 'best'
   * intersection easier. In generally, 'less' is overriding result.
   */
  bool operator <(const forced_positive<T>& fp) const
  {
    if (!this->is())
      return false;

    return !fp.is() || (this->_value < fp._value);
  }

  bool operator <=(const forced_positive<T>& fp) const
  {
    if (!fp.is())
      return true;

    return this->is() && (this->_value < fp._value);
  }

  bool operator <(const T& f) const
  {
    return *this < forced_positive<T>{f};
  }

  bool operator <=(const T& f) const
  {
    return *this <= forced_positive<T>{f};
  }

  static forced_positive<T> none;
};

template <typename T>
forced_positive<T> forced_positive<T>::none;

template <typename T>
bool operator <(const T& t, const forced_positive<T>& fp)
{
  return forced_positive<T>{t} < fp;
}

template <typename T>
bool operator <=(const T& t, const forced_positive<T>& fp)
{
  return forced_positive<T>{t} <= fp;
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
