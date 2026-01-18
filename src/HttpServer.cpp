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
                HttpResponse resp;
                std::string html =
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
                resp.set_header("Content-Type","text/html");
                resp.set_body(html);
                conn->send(resp.to_string());
            }
            break;
        }
    }
}

void HttpServer::start(){
    tcpServer_.start();
}