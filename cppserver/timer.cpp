
#include "timer.h"

#include <sys/time.h>

namespace cppserver_core {

size_t TimerItem::current_time_ = 0;

TimerItem::TimerItem(size_t timeout) : deleted_(false) {
  UpdateCurrentTime();
  expire_time_ = current_time_ + timeout;
}

TimerItem::~TimerItem() { deleted_ = true; }

void TimerItem::UpdateCurrentTime() {
  struct timeval cur;
  gettimeofday(&cur, NULL);
  current_time_ = (cur.tv_sec * 1000) + (cur.tv_usec / 1000);
}

TimerManager::TimerManager(size_t expire_time)
    : expire_time_threshold_(expire_time) {}

void TimerManager::AddTimer(TimerItem::ptr timer) {
  std::lock_guard<std::mutex> lock(queue_mtx_);
  timer_queue_.push(timer);
}

void TimerManager::SetExpiretime(size_t expire_time) {
  expire_time_threshold_ = expire_time;
}

size_t TimerManager::GetExpireTime() { return expire_time_threshold_; }

void TimerManager::HandleExpiredTimer() {}

}  // namespace cppserver_core