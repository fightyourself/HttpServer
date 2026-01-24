#pragma once
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include <functional>
#include "Connection.h"
class Epoll;
class EventLoop;

class Channel{
private:
    EventLoop * loop_;
    const int fd_;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
    bool isInpoll_ = false;
    std::function<void()>readCallback_;
    std::function<void()>disconnectCb_;
    std::function<void()>errorCb_;
    std::function<void()>writeCb_;
public:
    Channel(int fd,EventLoop * loop);
    ~Channel();
    int fd() const;
    uint32_t events() const;
    uint32_t revents() const;
    bool is_inpoll() const;

    void set_et();
    void set_revents(uint32_t ev);
    void set_isInpoll();
    void enable_read();
    void enable_write();
    void disable_write();
    void disalbe_all();

    void set_read_callback(std::function<void()>func);
    void set_disconnect_cb(std::function<void()>func);
    void set_error_cb(std::function<void()>func);
    void set_write_cb(std::function<void()>func);

    void handle();
};