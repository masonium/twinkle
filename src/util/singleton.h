#pragma once

#include <memory>
#include <mutex>

using std::call_once;
using std::shared_ptr;
using std::make_shared;

template <typename T>
class Singleton
{
public:
  static T& instance()
  {
    static T _instance;
    return _instance;
  }
};
