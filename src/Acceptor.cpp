#include "Acceptor.h"


Acceptor::Acceptor(const std::string &ip, uint16_t port,EventLoop *loop):listenSock_(create_nonblock_sock()),loop_(loop){
    if(listenSock_->fd()<0){
        printf("create socket fail\n");
        exit(-1);
    }
    listenSock_->set_keep_alive(true);
    listenSock_->set_reuse_addr(true);
    listenSock_->set_reuse_port(true);
    listenSock_->set_tcp_nodelay(true);
    InetAddress servAddr(ip,port);
    listenSock_->bind(servAddr);
    listenSock_->listen();
    chan_= new Channel(listenSock_->fd(),loop_);
    chan_->set_et();
    chan_->set_read_callback(std::bind(&Acceptor::new_connection,this));
    chan_->enable_read();
}

Acceptor::~Acceptor(){
    delete listenSock_;
    delete chan_;
}

Socket * Acceptor::sock()const{
    return listenSock_;
}

void Acceptor::new_connection(){
    while(true){
        InetAddress clientAddr;
        Socket * clientSock = new Socket(listenSock_->accept(clientAddr));
        if(clientSock->fd()<0 && errno == EAGAIN) break;
        clientSock->set_ip(clientAddr.ip());
        clientSock->set_port(clientAddr.port());
        newConnectionCb_(clientSock);
    }
}


void Acceptor::set_new_connnection_cb(std::function<void(Socket *)> func){
    newConnectionCb_ = func;
}