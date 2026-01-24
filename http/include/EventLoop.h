#pragma once
#include "Epoll.h"
#include <memory>
class Epoll;

class EventLoop{
private:
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop *)> epollTimeoutCb_;
public:
    EventLoop();
    ~EventLoop();
    void run();
    Epoll * ep() const;
    void set_epoll_timeout_cb(std::function<void(EventLoop *)> func);

};