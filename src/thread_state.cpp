#include "thread_state.h"
#include "film.h"
#include <iostream>
using std::cerr;
using std::endl;

class ThreadState
{
public:
  ThreadState(const Film& film) : f(film.clone()) { }

  Film& film() { return f; }
  const Film& film() const { return f; }

private:
  Film f;
};

class ThreadStateManager
{
public:
  ThreadStateManager(const Film& f) : _film(f) { }

  void register_thread();

  Film& get_film();

  void merge_films(Film & f) const;

private:
  const Film& _film;
  unordered_map<std::thread::id, ThreadState> _state_map;
};

void ThreadStateManager::register_thread()
{
  _state_map.emplace(std::this_thread::get_id(), _film.clone());
}

Film& ThreadStateManager::get_film()
{
  return _state_map.at(std::this_thread::get_id()).film();
}

void ThreadStateManager::merge_films(Film& f) const
{
  for (const auto& e: _state_map)
  {
    cerr << e.second.film().samples << "\n";
    f.merge(e.second.film());
  }
}

static shared_ptr<ThreadStateManager> _manager;

void register_thread_state_manager(const Film& f)
{
  _manager = make_shared<ThreadStateManager>(f);
}

void register_thread()
{
  _manager->register_thread();
}

Film& get_thread_film()
{
  return _manager->get_film();
}

void merge_thread_films(Film& f)
{
  _manager->merge_films(f);
}
