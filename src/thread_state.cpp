#include "thread_state.h"
#include "film.h"
#include <iostream>
using std::cerr;
using std::endl;
using std::make_unique;

class ThreadState
{
public:
  ThreadState() : f(0, 0) {}

  ThreadState(int width, int height, const std::string& lua_fn) :
    f(width, height), _runner(lua_fn) { }

  Film& film() { return f; }
  const Film& film() const { return f; }

  LuaRunner& runner() { return _runner; }

private:
  Film f;
  LuaRunner _runner;
};

class ThreadStateManager
{
public:
  ThreadStateManager(const Film& f, const string& s) : _film(f), boostrap_filename(s) { }

  void register_thread();

  Film& get_film();
  LuaRunner& get_lua_runner();

  void merge_films(Film & f) const;

private:
  const Film& _film;
  std::string boostrap_filename;
  std::mutex _state_map_mutex;
  unordered_map<std::thread::id, unique_ptr<ThreadState>> _state_map;
};

void ThreadStateManager::register_thread()
{
  std::lock_guard<std::mutex> lg(_state_map_mutex);
  _state_map.emplace(std::this_thread::get_id(),
                     make_unique<ThreadState>(_film.width, _film.height, boostrap_filename));
}

Film& ThreadStateManager::get_film()
{
  return _state_map.at(std::this_thread::get_id())->film();
}

LuaRunner& ThreadStateManager::get_lua_runner()
{
  return _state_map.at(std::this_thread::get_id())->runner();
}

void ThreadStateManager::merge_films(Film& f) const
{
  for (const auto& e: _state_map)
  {
    f.merge(e.second->film());
  }
}

static shared_ptr<ThreadStateManager> _manager;

void register_thread_state_manager(const Film& f, const std::string& lua_file)
{
  _manager = make_shared<ThreadStateManager>(f, lua_file);
}

void register_thread()
{
  _manager->register_thread();
}

Film& get_thread_film()
{
  return _manager->get_film();
}

LuaRunner& get_thread_lua_runner()
{
  return _manager->get_lua_runner();
}

void merge_thread_films(Film& f)
{
  _manager->merge_films(f);
}
