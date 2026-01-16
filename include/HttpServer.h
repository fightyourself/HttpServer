#pragma once
#include "TcpServer.h"
class HttpServer{
private:
    TcpServer tcpServer;
public:
    HttpServer();
    ~HttpServer();
    
};