#pragma once
#include "Epoll.h"
#include "Channel.h"
#include <memory>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <queue>
#include <mutex>
class Epoll;
class Channel;

class EventLoop{
private:
    std::unique_ptr<Epoll> ep_;
    pid_t runThreadId_;
    std::unique_ptr<Channel> eventChannel;
    std::queue<std::function<void()>>tasks_;
    std::mutex tasksMtx_;
    std::function<void(EventLoop *)> epollTimeoutCb_;
public:
    EventLoop();
    ~EventLoop();
    void run();
    Epoll * ep() const;
    void set_epoll_timeout_cb(std::function<void(EventLoop *)> func);
    bool isInRunThread() const;
    void notify_send_event(std::function<void()>func);
    void handle_send_event();
};