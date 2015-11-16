#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <queue>
#include <memory>
#include "thread_state.h"

using std::istream;
using std::ostream;
using std::shared_ptr;
using std::unique_ptr;

class LocalTask
{
public:
  virtual void run(uint worker_id) = 0;

  virtual ~LocalTask() {}
};

/**
 * A scheduler is responsible for registering each thread it uses.
 */

/* abstract */
class Scheduler
{
public:
  /* add a task to be scheduled */
  virtual void add_task(shared_ptr<LocalTask> t) = 0;

  /* blocks until all current tasks are complete. */
  virtual void complete_pending() = 0;

  /* Return the concurrency available to the scheduler. */
  virtual uint concurrency() const = 0;

  virtual ~Scheduler() { }
};

unique_ptr<Scheduler> make_scheduler(uint num_threads);
