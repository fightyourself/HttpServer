#include "HttpServer.h"

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
            HttpRequest *req = new HttpRequest;
            while(conn->parser().parse(req)){
                printf("method = %s,path = %s, version = %s\n",req->method().c_str(),req->path().c_str(),req->version().c_str());
                printf("query string:%s\n",req->queryString().c_str());
                router_.handle(req,conn);
            }
            break;
        }
    }
}

void HttpServer::start(){
    tcpServer_.start();
}

void HttpServer::GET(const std::string& pattern,std::function<void(HttpRequest *req,Connection *conn)> handler){
    router_.GET(pattern,handler);
}
    
void HttpServer::POST(const std::string& pattern,std::function<void(HttpRequest *req,Connection *conn)> handler){
    router_.POST(pattern,handler);
}
