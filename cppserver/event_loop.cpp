#include "event_loop.h"

#include <vector>

#include "channel.h"
#include "epoll_class.h"
#include "thread_pool.h"

namespace cppserver_core {

EventLoop::EventLoop() : ep_(nullptr) { ep_ = new Epoll(); }

EventLoop::~EventLoop() { delete ep_; }

void EventLoop::Loop() {
  while (!quit_) {
    std::vector<Channel*> channels = ep_->Poll();
    for (auto& channel : channels) {
      channel->HandleEvent();
    }
  }
}

void EventLoop::UpdateChannel(Channel* channel) { ep_->UpdateChannel(channel); }

}  // namespace cppserver_core
