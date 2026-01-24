#include "Acceptor.h"


Acceptor::Acceptor(const std::string &ip, uint16_t port,EventLoop *loop):loop_(loop),listenSock_(*create_nonblock_sock()),chan_(listenSock_.fd(),loop_){
    if(listenSock_.fd()<0){
        printf("create socket fail\n");
        exit(-1);
    }
    listenSock_.set_keep_alive(true);
    listenSock_.set_reuse_addr(true);
    listenSock_.set_reuse_port(true);
    listenSock_.set_tcp_nodelay(true);
    InetAddress servAddr(ip,port);
    listenSock_.bind(servAddr);
    listenSock_.listen();
    chan_.set_et();
    chan_.set_read_callback(std::bind(&Acceptor::new_connection,this));
    chan_.enable_read();
}

Acceptor::~Acceptor(){

}

Socket * Acceptor::sock(){
    return &listenSock_;
}

void Acceptor::new_connection(){
    // printf("acceptor::new connection\n");
    while(true){
        InetAddress clientAddr;
        std::unique_ptr<Socket> clientSock(new Socket(listenSock_.accept(clientAddr)));
        if(clientSock->fd()<0 && errno == EAGAIN) break;
        clientSock->set_ip(clientAddr.ip());
        clientSock->set_port(clientAddr.port());
        newConnectionCb_(std::move(clientSock));
    }
}


void Acceptor::set_new_connnection_cb(std::function<void(std::unique_ptr<Socket>)> func){
    newConnectionCb_ = func;
}