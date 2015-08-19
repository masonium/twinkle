#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <future>
#include <queue>

using std::istream;
using std::ostream;

class LocalTask
{
public:
  virtual void run() = 0;

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
  virtual void add_task(LocalTask* t, ScheduleHint hint) = 0;

  /* blocks until all current tasks are complete. */
  virtual void complete_pending() = 0;

  virtual int tasks_added() const = 0;
  virtual int tasks_completed() const = 0;

  virtual int tasks_pending() const = 0;

  virtual ~Scheduler() { }
};


class LocalThreadScheduler : public Scheduler
{
public:
  LocalThreadScheduler(uint num_threads_ = 1);

  void add_task(LocalTask* t, ScheduleHint hint) override;

  /* blocks until all current tasks are complete. */
  void complete_pending() override;

  ~LocalThreadScheduler();

private:
  static void worker(LocalThreadScheduler* sched, int worker_id,
                     std::future<int>&& fut);

  void on_task_started(int worker_id, LocalTask* task);
  void on_task_completed(int worker_id, LocalTask* task);

  std::queue<LocalTask*> task_queue;
  std::condition_variable queue_cv;
  std::mutex queue_mutex;

  std::condition_variable status_cv;
  std::mutex status_mutex;
  std::atomic<uint> num_threads_free;

  std::vector<std::thread> pool;
  std::vector<std::promise<int>> worker_signals;
};
