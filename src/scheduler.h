#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <queue>
#include <memory>

using std::istream;
using std::ostream;
using std::shared_ptr;

class LocalTask
{
public:
  virtual void run(uint worker_id) = 0;

  virtual ~LocalTask() {}
};

enum ScheduleHint
{
  SCHEDULE_HINT_NONE = 0,
  SCHEDULE_HINT_PREFER_LOCAL,
  SCHEDULE_HINT_PREFER_REMOTE
};

/* abstract */
class Scheduler
{
public:
  /* add a task to be scheduled */
  virtual void add_task(shared_ptr<LocalTask> t) = 0;

  /* blocks until all current tasks are complete. */
  virtual void complete_pending() = 0;
/*
  virtual int tasks_added() const = 0;
  virtual int tasks_completed() const = 0;

  virtual int tasks_pending() const = 0;
*/
  virtual ~Scheduler() { }
};

shared_ptr<Scheduler> make_scheduler(uint num_threads);
