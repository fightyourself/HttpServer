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
    for(auto &it:connections_){
        delete it.second;
    }
}

void TcpServer::start(){
    mainLoop_->run();
    for(auto &loop :subLoops_){
        loop->run();
    }
}

void TcpServer::connection_new(Socket *clientSock){
    Connection *connection = new Connection(clientSock,subLoops_[clientSock->fd()%3]);
    connection->set_connection_close_cb(std::bind(&TcpServer::connection_close,this,connection));
    connection->set_connection_error_cb(std::bind(&TcpServer::handle_error_connection,this,connection));
    connection->set_on_message_cb(std::bind(&TcpServer::on_message,this,std::placeholders::_1,std::placeholders::_2));
    connection->set_send_over_cb(std::bind(&TcpServer::send_over,this,std::placeholders::_1));
    connections_[connection->fd()] = connection;
    printf("client(fd=%d,ip=%s,port=%d) connect\n",clientSock->fd(),clientSock->ip().c_str(),clientSock->port());
}

void TcpServer::connection_close(Connection *conn){
    printf("client(fd=%d,ip=%s,port=%d) disconnect\n",conn->fd(),conn->ip().c_str(),conn->port());
    connections_.erase(conn->fd());
    delete conn;
}

void TcpServer::handle_error_connection(Connection *conn){
    printf("event error\n");
    connections_.erase(conn->fd());
    delete conn;
}

void TcpServer::on_message(Connection *conn,std::string &message){
    printf("recv(%d):%s\n",conn->fd(),message.c_str());
    conn->send(message.data(),message.size());
}

void TcpServer::send_over(Connection *conn){
    printf("send(%d) over\n",conn->fd());
}

void TcpServer::epoll_timeout(EventLoop *loop){
    printf("epoll timeout\n");
}