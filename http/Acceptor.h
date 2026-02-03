#pragma once
#include "Channel.h"
#include "Socket.h"
#include <memory>
class EventLoop;
class Acceptor{
private:
    EventLoop *loop_;
    Socket listenSock_;
    Channel chan_;
    std::function<void(std::unique_ptr<Socket>)> newConnectionCb_;
public:
    Acceptor(const std::string &ip,uint16_t port,EventLoop *loop);
    ~Acceptor();
    Socket * sock();
    void new_connection();
    void set_new_connnection_cb(std::function<void(std::unique_ptr<Socket>)> func);
};