#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,uint16_t port):mainLoop_(),subLoops_(),acceptor_(ip,port,&mainLoop_),mtx_(),
connections_(),ioThreadPool_(3,"io"),tcpReadCb_(){
    acceptor_.set_new_connnection_cb(std::bind(&TcpServer::connection_new,this,std::placeholders::_1));
    mainLoop_.set_epoll_timeout_cb(std::bind(&TcpServer::epoll_timeout,this,std::placeholders::_1));
    for(int i =0;i<3;i++){
        subLoops_.emplace_back(new EventLoop(true));
        subLoops_[i]->set_epoll_timeout_cb(std::bind(&TcpServer::epoll_timeout,this,std::placeholders::_1));
        subLoops_[i]->set_connection_close_cb(std::bind(&TcpServer::connection_close,this,std::placeholders::_1));
        ioThreadPool_.add_task(std::bind(&EventLoop::run,subLoops_[i].get()));
    }
}

TcpServer::~TcpServer(){

}

void TcpServer::start(){
    mainLoop_.run();
    for(auto &loop :subLoops_){
        loop->run();
    }
}

void TcpServer::connection_new(std::unique_ptr<Socket> clientSock){
    printf("client(fd=%d,ip=%s,port=%d) connect\n",clientSock->fd(),clientSock->ip().c_str(),clientSock->port());
    int fd = clientSock->fd();
    spConnection connection = std::make_shared<Connection>(std::move(clientSock),subLoops_[fd%3].get());
    connection->set_connection_close_cb(std::bind(&TcpServer::connection_close,this,std::placeholders::_1));
    connection->set_connection_error_cb(std::bind(&TcpServer::handle_error_connection,this,std::placeholders::_1));
    connection->set_connection_read_cb(std::bind(&TcpServer::tcp_read,this,std::placeholders::_1));
    connection->set_send_over_cb(std::bind(&TcpServer::send_over,this,std::placeholders::_1));
    connections_[connection->fd()] = connection;
    subLoops_[fd%3]->queue_in_loop([=](){
        this->subLoops_[fd%3]->add_connection(connection);
    });
}

void TcpServer::connection_close(spConnection conn){
    printf("client(fd=%d,ip=%s,port=%d) disconnect\n",conn->fd(),conn->ip().c_str(),conn->port());
    mainLoop_.queue_in_loop([=](){
        connections_.erase(conn->fd());
        conn->remove_channel_from_loop();
        conn->set_is_closed();
    });
}

void TcpServer::handle_error_connection(spConnection conn){
    printf("event error\n");
    connection_close(conn);
}


void TcpServer::tcp_read(spConnection conn){
    tcpReadCb_(conn);
}

void TcpServer::send_over(spConnection conn){
    // printf("send(%d) over\n",conn->fd());
}

void TcpServer::epoll_timeout(EventLoop *loop){
    printf("epoll timeout\n");
}

void TcpServer::set_tcp_read_cb(std::function<void(spConnection)> func){
    tcpReadCb_ = func;
}