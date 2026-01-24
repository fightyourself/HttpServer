#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,uint16_t port){
    ioThreadPool_ = new ThreadPool(3);
    mainLoop_ = new EventLoop;
    acceptor_ = new Acceptor(ip,port,mainLoop_);
    acceptor_->set_new_connnection_cb(std::bind(&TcpServer::connection_new,this,std::placeholders::_1));
    mainLoop_->set_epoll_timeout_cb(std::bind(&TcpServer::epoll_timeout,this,std::placeholders::_1));
    for(int i =0;i<3;i++){
        subLoops_.push_back(new EventLoop);
        subLoops_[i]->set_epoll_timeout_cb(std::bind(&TcpServer::epoll_timeout,this,std::placeholders::_1));
        ioThreadPool_->add_task(std::bind(&EventLoop::run,subLoops_[i]));
    }

}

TcpServer::~TcpServer(){
    delete mainLoop_;
    delete acceptor_;
    for (auto &loop:subLoops_){
        delete loop;
    }
    
}

void TcpServer::start(){
    mainLoop_->run();
    for(auto &loop :subLoops_){
        loop->run();
    }
}

void TcpServer::connection_new(Socket *clientSock){
    spConnection connection(new Connection(clientSock,subLoops_[clientSock->fd()%3]));
    connection->set_connection_close_cb(std::bind(&TcpServer::connection_close,this,std::placeholders::_1));
    connection->set_connection_error_cb(std::bind(&TcpServer::handle_error_connection,this,std::placeholders::_1));
    connection->set_connection_read_cb(std::bind(&TcpServer::tcp_read,this,std::placeholders::_1));
    connection->set_send_over_cb(std::bind(&TcpServer::send_over,this,std::placeholders::_1));
    connections_[connection->fd()] = connection;
    printf("client(fd=%d,ip=%s,port=%d) connect\n",clientSock->fd(),clientSock->ip().c_str(),clientSock->port());
}

void TcpServer::connection_close(spConnection conn){
    printf("client(fd=%d,ip=%s,port=%d) disconnect\n",conn->fd(),conn->ip().c_str(),conn->port());
    connections_.erase(conn->fd());
    conn->remove_channel_from_loop();
    conn->set_is_closed();
}

void TcpServer::handle_error_connection(spConnection conn){
    printf("event error\n");
    connections_.erase(conn->fd());
    conn->remove_channel_from_loop();
    conn->set_is_closed();
}

void TcpServer::tcp_read(spConnection conn){
    tcpReadCb_(conn);
}

void TcpServer::send_over(spConnection conn){
    printf("send(%d) over\n",conn->fd());
}

void TcpServer::epoll_timeout(EventLoop *loop){
    printf("epoll timeout\n");
}

void TcpServer::set_tcp_read_cb(std::function<void(spConnection)> func){
    tcpReadCb_ = func;
}