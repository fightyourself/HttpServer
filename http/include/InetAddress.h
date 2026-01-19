#pragma once
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
class InetAddress{
private:
    struct sockaddr_in addr_;
public:
    InetAddress();
    InetAddress(const std::string & ip, uint16_t port);
    InetAddress(const sockaddr_in& addr);
    ~InetAddress();
    const char * ip() const;
    uint16_t port() const;
    const sockaddr * addr() const;
    void set_addr(const sockaddr_in & addr);
};