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
    std::map<int,Connection *> connections_;
    ThreadPool *ioThreadPool_;
    std::function<void(Connection *)> tcpReadCb_;
public:
    TcpServer(const std::string& ip,uint16_t port);
    ~TcpServer();
    void start();
    void connection_new(Socket *clientSock);
    void connection_close(Connection *conn);
    void handle_error_connection(Connection *conn);
    void tcp_read(Connection *conn);
    void send_over(Connection *conn);
    void epoll_timeout(EventLoop *loop);

    void set_tcp_read_cb(std::function<void(Connection *)> func);
};