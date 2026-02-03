#pragma once
#include "TcpServer.h"
#include "Router.h"
#include "ThreadPool.h"
class HttpServer{
private:
    TcpServer tcpServer_;
    Router router_;
    ThreadPool workThreadPool_;
public:
    HttpServer(const std::string& ip,uint16_t port);
    ~HttpServer(); 

    void GET(const std::string& pattern,std::function<void(HttpRequest *req,spConnection conn)> handler);
    void POST(const std::string& pattern,std::function<void(HttpRequest *req,spConnection conn)> handler);

    void handle_tcp_read(spConnection conn);
    void start();
};