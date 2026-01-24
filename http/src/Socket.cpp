#include "../include/Socket.h"
/*
class Socket{
private:
    const int fd_;
public:
    Socket(int fd);
    ~Socket();
    void set_reuse_addr(bool on);
    void set_tcp_nodelay(bool on);
    void set_reuse_port(bool on);
    void set_keep_alive(bool on);
    void bind() const;
    void listen() const;
    int accept() const;
};
*/

Socket::Socket(int fd):fd_(fd),ip_(),port_(){

}

Socket::~Socket(){
    ::close(fd_);
}

void Socket::set_reuse_addr(bool on){
    int opt = on?1:0;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof(opt)));
}

void Socket::set_reuse_port(bool on){
    int opt = on?1:0;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&opt,static_cast<socklen_t>(sizeof(opt)));
}

void Socket::set_keep_alive(bool on){
    int opt = on?1:0;
    setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&opt,static_cast<socklen_t>(sizeof(opt)));
}
void Socket::set_tcp_nodelay(bool on){
    int opt = on?1:0;
    setsockopt(fd_,SOL_SOCKET,TCP_NODELAY,&opt,static_cast<socklen_t>(sizeof(opt)));
}

void Socket::set_ip(const std::string &ip){
    ip_ = ip;
}

void Socket::set_port(uint16_t port){
    port_ = port;
}


void Socket::bind(const InetAddress &addr){
    if(::bind(fd_,addr.addr(),sizeof(sockaddr))<0){
        printf("%s:%s:%d bind error:%d",__FILE__,__FUNCTION__,__LINE__,errno);
        exit(-1);
    }
    ip_ = addr.ip();
    port_ = addr.port();
}

void Socket::listen(int nn) const {
    if(::listen(fd_,nn)<0){
        printf("%s:%s:%d listen error:%d",__FILE__,__FUNCTION__,__LINE__,errno);
        exit(-1);
    }
}

int Socket::accept(InetAddress &addr) const{
    sockaddr_in peerAddr;
    socklen_t len = sizeof(peerAddr);
    int sockfd = accept4(fd_,(struct sockaddr*)&peerAddr,&len,SOCK_NONBLOCK);
    if(sockfd<0 && errno!=EAGAIN) {
        printf("%s:%s:%d accept error:%d",__FILE__,__FUNCTION__,__LINE__,errno);
        return -1;
    }
    addr.set_addr(peerAddr);
    return sockfd;
}

int Socket::fd() const {
    return fd_;
}

std::string Socket::ip() const{
    return ip_;
}

uint16_t Socket::port() const{
    return port_;
}

Socket * create_nonblock_sock(){
    return new Socket(::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0));
}