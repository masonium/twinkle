#include "scheduler.h"
#include "util.h"
#include <future>
#include <thread>
#include <chrono>

using std::future;
using std::unique_lock;
using std::mutex;

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

void LocalThreadScheduler::add_task(shared_ptr<SchedulerTask> task, ScheduleHint hint)
{
  {
    unique_lock<std::mutex> lock(queue_mutex);
    task_queue.push(task);
  }
  ++pending_task_count;
}


void LocalThreadScheduler::add_task(shared_ptr<LocalTask> task, ScheduleHint hint)
{
  add_task(std::make_shared<LocalTaskWrapper>(task), hint);
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
