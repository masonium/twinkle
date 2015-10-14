#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <unordered_map>

using std::unordered_map;
using std::shared_ptr;
using std::make_shared;

/*
template <typename T>
class Singleton
{
public:
  static T& instance()
  {
    if (!_instance)
    {
      call_once(_flag, [](){ _instance = make_shared<T>(); });
    }
    return *_instance;
  }

private:
  static shared_ptr<T> _instance;
  static std::once_flag _flag;
};
*/

class Film;
class ThreadState;
class ThreadStateManager;

void register_thread_state_manager(const Film& f);
void register_thread();
Film& get_thread_film();
void merge_thread_films(Film& f);
