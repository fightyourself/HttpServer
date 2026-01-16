#include "EventLoop.h"
EventLoop::EventLoop():ep_(new Epoll){

}

EventLoop::~EventLoop(){
    delete ep_;
}

void EventLoop::run(){
    while(true){
        std::vector<Channel *> chs = ep_->loop();
        if(chs.size()==0) epollTimeoutCb_(this);
        for(auto *ch:chs){
            ch->handle();
        }
    }
}

Epoll * EventLoop::ep() const{
    return ep_;
}

void EventLoop::set_epoll_timeout_cb(std::function<void(EventLoop *)> func){
    epollTimeoutCb_ = func;
}