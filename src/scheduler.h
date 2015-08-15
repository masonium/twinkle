#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <queue>

using std::istream;
using std::ostream;

class Task
{
public:
  virtual void run() = 0;

  virtual void serialize(ostream&) = 0;
  virtual void deserializer(istream&) = 0;
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
  virtual void add_task(const Task* t, ScheduleHint hint) = 0;

  virtual int tasks_added() const = 0;
  virtual int tasks_completed() const = 0;

  virtual int tasks_pending() const = 0;
private:
};


class LocalThreadScheduler : public Scheduler
{
public:
  LocalThreadScheduler(int num_threads_ = 1);

  void add_task(const Task* t, ScheduleHint hint) override;

  int tasks_added() const override;
  int tasks_completed() const override;
  int tasks_pending() const override;

private:
  std::queue<Task*> task_queue;

  std::vector<std::thread> pool;
};
