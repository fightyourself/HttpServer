#pragma once
#include "TcpServer.h"
#include "HttpResponse.h"
#include "Router.h"
class HttpServer{
private:
    TcpServer tcpServer_;
    Router router_;
public:
    HttpServer(const std::string& ip,uint16_t port);
    ~HttpServer(); 

    void GET(const std::string& pattern,std::function<void(HttpRequest *req,Connection *conn)> handler);
    void POST(const std::string& pattern,std::function<void(HttpRequest *req,Connection *conn)> handler);

    void handle_tcp_read(Connection *conn);
    void start();
};