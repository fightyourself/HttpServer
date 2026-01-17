#include "../include/HttpServer.h"

HttpServer::HttpServer(const std::string& ip,uint16_t port):tcpServer_(ip,port){
    tcpServer_.set_tcp_read_cb(std::bind(&HttpServer::handle_tcp_read,this,std::placeholders::_1));
}

HttpServer::~HttpServer(){

}

void HttpServer::handle_tcp_read(Connection *conn){
    char buf[4096];
    while(true){
        bzero(&buf,sizeof(buf));
        int readn = recv(conn->fd(),buf,sizeof(buf),0);
        if(readn>0){
            conn->parser().append(buf,readn);
        }else if(readn==-1 && errno == EINTR){
            continue;
        }else if(readn==-1 && (errno==EAGAIN||errno == EWOULDBLOCK)){
            HttpRequest req;
            while(conn->parser().parse(&req)){
                printf("method = %s,path = %s, version = %s\n",req.method(),req.path(),req.version());
                std::string html =
                    "HTTP/1.1 200 OK\r\n"
                    "Date: Sat, 17 Jan 2026 23:16:00 GMT\r\n"
                    "Server: MySimpleServer/1.0\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n"
                    "Content-Length: 196\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "<!DOCTYPE html>\r\n"
                    "<html>\r\n"
                    "<head>\r\n"
                    "    <title>Hello World</title>\r\n"
                    "</head>\r\n"
                    "<body>\r\n"
                    "    <h1>Welcome to My Simple Page!</h1>\r\n"
                    "    <p>This is a basic HTML page served via HTTP.</p>\r\n"
                    "</body>\r\n"
                    "</html>\r\n";
                conn->send(html.data(),html.size());
            }
            break;
        }
    }
}

void HttpServer::start(){
    tcpServer_.start();
}