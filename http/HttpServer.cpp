#include "HttpServer.h"
#include "constants.h"
#include "Connection.h"
#include <sys/socket.h>
HttpServer::HttpServer(const std::string& ip,uint16_t port):tcpServer_(ip,port),router_(),workThreadPool_(workThreadNum,"work"){
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
            conn->update_time_stamp();
            while(conn->parser().parse()){
                HttpRequest req = conn->parser().req();
                if(workThreadPool_.size()>0){
                    workThreadPool_.add_task([=]()mutable{
                        // printf("method = %s,path = %s, version = %s\n",req.method().c_str(),req.path().c_str(),req.version().c_str());
                        // printf("query string:%s\n",req->queryString().c_str());
                        router_.handle(&req,conn);
                    });
                }else{
                    HttpRequest req = conn->parser().req();
                    // printf("method = %s,path = %s, version = %s\n",req.method().c_str(),req.path().c_str(),req.version().c_str());
                    // printf("query string:%s\n",req->queryString().c_str());
                    router_.handle(&req,conn);
                }
                conn->parser().clear_req();
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
