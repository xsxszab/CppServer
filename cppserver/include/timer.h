#ifndef TIMER_H
#define TIMER

#include <memory>
#include <mutex>
#include <queue>

namespace cppserver_core {

class TimerItem {
 public:
  using ptr = std::shared_ptr<TimerItem>;
  TimerItem(size_t timeout);
  ~TimerItem();

  size_t GetExpireTime() { return expire_time_; }

  static void UpdateCurrentTime();

 private:
  static size_t current_time_;
  size_t expire_time_;

  bool deleted_;
};

struct TimeCmp {
  bool operator()(TimerItem::ptr& a, TimerItem::ptr& b) {
    return a->GetExpireTime() > b->GetExpireTime();
  }
};

class TimerManager {
 public:
  TimerManager(size_t expire_time);
  ~TimerManager() = default;

  void AddTimer(TimerItem::ptr timer);

  void SetExpiretime(size_t expire_time);
  size_t GetExpireTime();

  void HandleExpiredTimer();

 private:
  size_t expire_time_threshold_;

  std::priority_queue<TimerItem::ptr> timer_queue_;
  std::mutex queue_mtx_;
};

}  // namespace cppserver_core

#endif
