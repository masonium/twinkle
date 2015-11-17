#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <script/runner.h>

using std::unordered_map;
using std::shared_ptr;
using std::make_shared;


class Film;
class ThreadState;
class ThreadStateManager;

void register_thread_state_manager(const Film& f, const string& lua_f);
void register_thread();
Film& get_thread_film();
#if FEATURE_LUA_SCRIPTING
LuaRunner& get_thread_lua_runner();
#endif
void merge_thread_films(Film& f);
