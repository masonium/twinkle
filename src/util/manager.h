#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include "base.h"

using std::make_shared;
using std::shared_ptr;

/*
 * Utility class for holding objects of a given type, to be recovered later.
 */

class EntityManager
{
private:
  EntityManager() : _next_key(0) { }
  EntityManager(const EntityManager&) = delete;

public:
  using key_type = uint32_t;

  static EntityManager& instance()
  {
    static EntityManager _inst;
    return _inst;
  }

  key_type save(shared_ptr<Base> ptr)
  {
    key_type key = _next_key++;
    _entries.emplace(key, ptr);
    return key;
  }

  template <typename T, typename... Args>
  shared_ptr<T> create(Args&&... args)
  {
    key_type key = _next_key++;

    auto s = make_shared<T>(std::forward<Args>(args)...);
    _entries.emplace(key, s);

    return s;
  }

  void clear()
  {
    _entries.clear();
    _next_key = 0;
  }

  Base* operator [](key_type key)
  {
    return _entries[key].get();
  }
  Base* at(key_type key)
  {
    return _entries.at(key).get();
  }

  void release(key_type key)
  {
    _entries.erase(key);
  }

private:
  key_type _next_key;
  std::unordered_map<key_type, std::shared_ptr<Base>> _entries;
};

template <typename T, typename... Args>
shared_ptr<T> make_entity(Args&&... args)
{
  return EntityManager::instance().create<T>(std::forward<Args>(args)...);
}
