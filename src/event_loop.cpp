#include "event_loop.h"

#include <vector>

#include "channel.h"
#include "epoll_class.h"
#include "thread_pool.h"

namespace cppserver_core {

EventLoop::EventLoop() : ep(nullptr), quit(false) { ep = new Epoll(); }

EventLoop::~EventLoop() { delete ep; }

void EventLoop::loop() {
  while (!quit) {
    std::vector<Channel*> channels = ep->poll();
    for (auto& channel : channels) {
      channel->handleEvent();
    }
  }
}

void EventLoop::updateChannel(Channel* channel) { ep->updateChannel(channel); }

}  // namespace cppserver_core
