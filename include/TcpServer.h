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
public:
    TcpServer(const std::string& ip,uint16_t port);
    ~TcpServer();
    void start();
    void connection_new(Socket *clientSock);
    void connection_close(Connection *conn);
    void handle_error_connection(Connection *conn);
    void on_message(Connection *conn,std::string &message);
    void send_over(Connection *conn);
    void epoll_timeout(EventLoop *loop);
};