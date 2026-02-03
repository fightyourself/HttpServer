#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string>
class InetAddress;
class Socket{
private:
    const int fd_;
    std::string ip_;
    uint16_t port_;
public:
    Socket(int fd);
    ~Socket();
    void set_reuse_addr(bool on);
    void set_tcp_nodelay(bool on);
    void set_reuse_port(bool on);
    void set_keep_alive(bool on);
    void bind(const InetAddress &addr);
    void listen(int nn=128) const;
    void set_ip(const std::string &ip);
    void set_port(uint16_t port);
    int accept(InetAddress &addr) const;
    int fd()const;
    std::string ip() const;
    uint16_t port() const;

};

Socket * create_nonblock_sock();