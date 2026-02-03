#pragma once 
#include <sys/epoll.h>
#include <vector>
#include <sys/types.h>
#include <errno.h>
#include <iostream>
#include <cstring>

class Channel;

class Epoll{
private:
    static const int MaxElement = 100;
    const int epollFd_;
    epoll_event events_[MaxElement];
public:
    Epoll();
    ~Epoll();
    void update_channel(Channel *ch) const;
    void remove_channel(Channel *ch) const;
    std::vector<Channel *> loop(int timeout=-1);
};