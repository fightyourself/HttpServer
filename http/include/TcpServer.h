#pragma once
#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <map>
class TcpServer{
private:
    EventLoop *mainLoop_;
    std::vector<EventLoop *> subLoops_;
    Acceptor *acceptor_;
    std::map<int,spConnection> connections_;
    ThreadPool *ioThreadPool_;
    std::function<void(spConnection)> tcpReadCb_;
public:
    TcpServer(const std::string& ip,uint16_t port);
    ~TcpServer();
    void start();
    void connection_new(Socket *clientSock);
    void connection_close(spConnection conn);
    void handle_error_connection(spConnection conn);
    void tcp_read(spConnection conn);
    void send_over(spConnection conn);
    void epoll_timeout(EventLoop *loop);

    void set_tcp_read_cb(std::function<void(spConnection)> func);
};