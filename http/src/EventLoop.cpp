#include "EventLoop.h"
EventLoop::EventLoop():ep_(new Epoll),runThreadId_(),eventChannel(new Channel(eventfd(0,EFD_NONBLOCK),this)),epollTimeoutCb_(){
    eventChannel->set_et();
    eventChannel->enable_read();
    eventChannel->set_read_callback(std::bind(&EventLoop::handle_send_event,this));
}

EventLoop::~EventLoop(){

}

void EventLoop::run(){
    // printf("EventLoop run in %ld\n",syscall(SYS_gettid));
    runThreadId_ = syscall(SYS_gettid);
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

bool EventLoop::isInRunThread() const{
    return syscall(SYS_gettid) == runThreadId_;
}

void EventLoop::notify_send_event(std::function<void()>func) {
    {
        std::unique_lock<std::mutex> mtx(tasksMtx_);
        tasks_.push(func);
    }
    uint64_t val = 1;
    write(eventChannel->fd(),&val,sizeof(uint64_t));
}

void EventLoop::handle_send_event(){
    uint64_t val;
    read(eventChannel->fd(),&val,sizeof(uint64_t));
    std::unique_lock<std::mutex> mtx(tasksMtx_);
    std::function<void()> fn;
    while(tasks_.size()>0){
        fn =std::move(tasks_.front());
        tasks_.pop();
        fn();
    }
}