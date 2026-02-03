#pragma once
#include <memory>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <queue>
#include <mutex>
#include <map>
#include <functional>
class Epoll;
class Channel;
class Connection;
using spConnection = std::shared_ptr<Connection>;
class EventLoop{
private:
    std::unique_ptr<Epoll> ep_;
    pid_t runThreadId_;
    std::unique_ptr<Channel> eventChannel;
    std::unique_ptr<Channel> timeChannel;
    std::queue<std::function<void()>>tasks_;
    std::mutex tasksMtx_;
    std::map<int,spConnection> conns_;
    std::function<void(EventLoop *)> epollTimeoutCb_;
    std::function<void(spConnection)> connectionCloseCb_;
public:
    EventLoop(bool isSub=false);
    ~EventLoop();
    void run();
    Epoll * ep() const;
    void set_epoll_timeout_cb(std::function<void(EventLoop *)> func);
    void set_connection_close_cb(std::function<void(spConnection)> func);
    void add_connection(spConnection conn);
    void remove_connection(spConnection conn);
    bool isInRunThread() const;



    void heart_beat();


    void queue_in_loop(std::function<void()>func);
    void handle_events();
};