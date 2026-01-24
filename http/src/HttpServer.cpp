#include "HttpServer.h"

HttpServer::HttpServer(const std::string& ip,uint16_t port):tcpServer_(ip,port),router_(),workThreadPool_(3,"work"){
    tcpServer_.set_tcp_read_cb(std::bind(&HttpServer::handle_tcp_read,this,std::placeholders::_1));
}

HttpServer::~HttpServer(){

}

void HttpServer::handle_tcp_read(spConnection conn){
    char buf[4096];
    while(true){
        bzero(&buf,sizeof(buf));
        int readn = recv(conn->fd(),buf,sizeof(buf),0);
        // printf("readn = %d\n",readn);
        if(readn>0){
            conn->parser().append(buf,readn);
        }else if(readn==-1 && errno == EINTR){
            continue;
        }else if(readn==-1 && (errno==EAGAIN||errno == EWOULDBLOCK)){
            HttpRequest *req = new HttpRequest;
            while(conn->parser().parse(req)){
                if(workThreadPool_.size()>0){
                    workThreadPool_.add_task([&](){
                        printf("method = %s,path = %s, version = %s\n",req->method().c_str(),req->path().c_str(),req->version().c_str());
                        // printf("query string:%s\n",req->queryString().c_str());
                        router_.handle(req,conn);
                    });
                }else{
                    printf("method = %s,path = %s, version = %s\n",req->method().c_str(),req->path().c_str(),req->version().c_str());
                    // printf("query string:%s\n",req->queryString().c_str());
                    router_.handle(req,conn);
                }
            }
            break;
        }else if(readn==0){
            tcpServer_.connection_close(conn);
            break;
        }
    }
}

void HttpServer::start(){
    tcpServer_.start();
}

void HttpServer::GET(const std::string& pattern,std::function<void(HttpRequest *,spConnection)> handler){
    router_.GET(pattern,handler);
}
    
void HttpServer::POST(const std::string& pattern,std::function<void(HttpRequest *req,spConnection)> handler){
    router_.POST(pattern,handler);
}
