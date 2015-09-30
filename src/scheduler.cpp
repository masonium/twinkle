#include "scheduler.h"
#include "util.h"
#include <future>
#include <thread>
#include <chrono>
#include <cassert>

using std::future;
using std::unique_lock;
using std::mutex;
using std::make_shared;

class LocalThreadScheduler : public Scheduler
{
public:
  LocalThreadScheduler(uint num_threads_ = 0);

  void add_task(shared_ptr<LocalTask> t) override;

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

  void add_task(shared_ptr<SchedulerTask> t);

  void on_task_started(int worker_id, const shared_ptr<SchedulerTask>& task);
  void on_task_completed(int worker_id, const shared_ptr<SchedulerTask>& task);

  std::queue<shared_ptr<SchedulerTask>> task_queue;
  std::mutex queue_mutex;
  std::atomic<uint> pending_task_count;

  std::atomic<uint> num_threads_free;

  std::vector<std::thread> pool;
};

/**
 * The blocking scheduler is the baseline, stupid scheduler. It runs tasks as
 * soon as they are added, on the same thread.
 */
class BlockingScheduler : public Scheduler
{
public:
  void add_task(shared_ptr<LocalTask> t) override
  {
    t->run(0);
  }

  void complete_pending() override { }

  ~BlockingScheduler() { }
};

void LocalThreadScheduler::worker(LocalThreadScheduler& scheduler, int worker_id)
{
  bool keep_running = true;
  while (keep_running)
  {
    uint task_count = scheduler.pending_task_count;
    if (task_count == 0)
    {
      std::this_thread::yield();
      continue;
    }

    if (std::atomic_compare_exchange_weak(&scheduler.pending_task_count, &task_count, task_count - 1))
    {
      std::unique_lock<std::mutex> lock(scheduler.queue_mutex);

      auto task = scheduler.task_queue.front();
      scheduler.task_queue.pop();
      lock.unlock();

      scheduler.on_task_started(worker_id, task);
      keep_running = task->run(worker_id);
      scheduler.on_task_completed(worker_id, task);
    }
  }
}

LocalThreadScheduler::LocalThreadScheduler(uint num_threads_) : pending_task_count(0)
{
  if (num_threads_ == 0)
    num_threads_ = num_system_procs();

  for (auto i = 0u; i < num_threads_; ++i)
    pool.emplace_back(LocalThreadScheduler::worker, std::ref(*this), i);

  num_threads_free = num_threads_;
}

void LocalThreadScheduler::add_task(shared_ptr<SchedulerTask> task)
{
  {
    unique_lock<std::mutex> lock(queue_mutex);
    task_queue.push(task);
  }
  ++pending_task_count;
}


void LocalThreadScheduler::add_task(shared_ptr<LocalTask> task)
{
  add_task(std::make_shared<LocalTaskWrapper>(task));
}

void LocalThreadScheduler::on_task_started(int worker_id, const shared_ptr<SchedulerTask>& task)
{
  --num_threads_free;
}

void LocalThreadScheduler::on_task_completed(int worker_id, const shared_ptr<SchedulerTask>& task)
{
  ++num_threads_free;
}

void LocalThreadScheduler::complete_pending()
{
  // wait for the queue to be empty
  while (true)
  {
    unique_lock<std::mutex> queue_lock(queue_mutex, std::try_to_lock);

    if (queue_lock && task_queue.empty())
      break;
  }

  // wait for all of the threads to be free
  while (num_threads_free != pool.size())
    std::this_thread::yield();
}

LocalThreadScheduler::~LocalThreadScheduler()
{
  for (auto& thread: pool)
    add_task(std::make_shared<SchedulerTask>());

  for (auto& thread: pool)
    thread.join();
}

////////////////////////////////////////////////////////////////////////////////

shared_ptr<Scheduler> make_scheduler(uint num_threads)
{
  assert(num_threads > 0);
  if (num_threads == 1)
  {
    return make_shared<BlockingScheduler>();
  }
  else
  {
    return make_shared<LocalThreadScheduler>(num_threads);
  }
}
