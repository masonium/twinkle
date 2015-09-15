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
  virtual void add_task(shared_ptr<LocalTask> t, ScheduleHint hint) = 0;

  /* blocks until all current tasks are complete. */
  virtual void complete_pending() = 0;
/*
  virtual int tasks_added() const = 0;
  virtual int tasks_completed() const = 0;

  virtual int tasks_pending() const = 0;
*/
  virtual ~Scheduler() { }
};


class LocalThreadScheduler : public Scheduler
{
public:
  LocalThreadScheduler(uint num_threads_ = 0);

  void add_task(shared_ptr<LocalTask> t, ScheduleHint hint = SCHEDULE_HINT_NONE) override;

  /* blocks until all current tasks are complete. */
  void complete_pending() override;

  ~LocalThreadScheduler();

private:
  class SchedulerTask
  {
  public:
    virtual bool run(uint worker)
    {
      return false;
    }
    SchedulerTask() { }
  };

  class LocalTaskWrapper : public SchedulerTask
  {
  public:
    LocalTaskWrapper(shared_ptr<LocalTask> task_) : task(task_) { }

    bool run(uint worker) override
    {
      task->run(worker);
      return true;
    }

  private:
    shared_ptr<LocalTask> task;
  };

  static void worker(LocalThreadScheduler& sched, int worker_id);

  void add_task(shared_ptr<SchedulerTask> t, ScheduleHint hint = SCHEDULE_HINT_NONE);

  void on_task_started(int worker_id, const shared_ptr<SchedulerTask>& task);
  void on_task_completed(int worker_id, const shared_ptr<SchedulerTask>& task);

  std::queue<shared_ptr<SchedulerTask>> task_queue;
  std::mutex queue_mutex;
  std::atomic<uint> pending_task_count;

  std::atomic<uint> num_threads_free;

  std::vector<std::thread> pool;
};
