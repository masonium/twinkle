#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include "util/singleton.h"

/*
 * Utility class for holding objects of a given type, to be recovered later.
 */

template <typename T>
class Manager : public Singleton<Manager<T>>
{
private:
  Manager() : _next_key(0) { }
  friend class Singleton<Manager<T>>;

public:
  using key_type = uint32_t;
  using value_type = std::shared_ptr<T>;

  key_type save(std::shared_ptr<T> ptr)
  {
    key_type key = _next_key++;
    _entries.emplace(key, ptr);
    return key;
  }

  void clear()
  {
    _entries.clear();
    _next_key = 0;
  }

  value_type operator [](key_type key)
  {
    return _entries[key];
  }
  value_type at(key_type key)
  {
    return _entries.at(key);
  }

  void release(key_type key)
  {
    _entries.erase(key);
  }

private:
  key_type _next_key;
  std::unordered_map<key_type, std::shared_ptr<T>> _entries;
};
