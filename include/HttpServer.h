#pragma once
#include "TcpServer.h"
class HttpServer{
private:
    TcpServer tcpServer_;
public:
    HttpServer(const std::string& ip,uint16_t port);
    ~HttpServer();

    void handle_tcp_read(Connection *conn);
    void start();
};