#include "EventLoop.h"
EventLoop::EventLoop():ep_(new Epoll),epollTimeoutCb_(){

}

EventLoop::~EventLoop(){

}
#include <sys/syscall.h>
void EventLoop::run(){
    // printf("EventLoop run in %ld\n",syscall(SYS_gettid));
    while(true){
        std::vector<Channel *> chs = ep_->loop();
        if(chs.size()==0) epollTimeoutCb_(this);
        for(auto *ch:chs){
            ch->handle();
        }
    }
}

Epoll * EventLoop::ep() const{
    return ep_.get();
}

void EventLoop::set_epoll_timeout_cb(std::function<void(EventLoop *)> func){
    epollTimeoutCb_ = func;
}