#pragma once
#include "EventLoop.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include <map>
#include <memory>
class Connection;
using spConnection = std::shared_ptr<Connection>;
class TcpServer{
private:
    EventLoop mainLoop_;
    std::vector<std::unique_ptr<EventLoop>>subLoops_;
    Acceptor acceptor_;
    std::mutex mtx_;
    std::map<int,spConnection> connections_;
    ThreadPool ioThreadPool_;
    std::function<void(spConnection)> tcpReadCb_;
public:
    TcpServer(const std::string& ip,uint16_t port);
    ~TcpServer();
    void start();
    void connection_new(std::unique_ptr<Socket> clientSock);
    void connection_close(spConnection conn);
    void handle_error_connection(spConnection conn);
    void tcp_read(spConnection conn);
    void send_over(spConnection conn);
    void epoll_timeout(EventLoop *loop);

    void set_tcp_read_cb(std::function<void(spConnection)> func);
};