#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "Connection.h"
#include <sys/timerfd.h>
EventLoop::EventLoop(bool isSub):ep_(new Epoll),runThreadId_(),eventChannel(new Channel(eventfd(0,EFD_NONBLOCK),this)),
timeChannel(new Channel(timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK),this)),epollTimeoutCb_(){
    eventChannel->set_et();
    eventChannel->enable_read();
    eventChannel->set_read_callback(std::bind(&EventLoop::handle_events,this));
    if(isSub){
        itimerspec timeout;
        memset(&timeout,0,sizeof(timeout));
        timeout.it_value.tv_sec = 5;
        timeout.it_value.tv_nsec = 0;
        timerfd_settime(timeChannel->fd(),0,&timeout,0);
        timeChannel->set_et();
        timeChannel->enable_read();
        timeChannel->set_read_callback(std::bind(&EventLoop::heart_beat,this));
    }
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

void EventLoop::set_connection_close_cb(std::function<void(spConnection)> func){
    connectionCloseCb_ = func;
}

void EventLoop::add_connection(spConnection conn){
    conns_[conn->fd()] = conn;
}

void EventLoop::remove_connection(spConnection conn){
    conns_.erase(conn->fd());
}


bool EventLoop::isInRunThread() const{
    return syscall(SYS_gettid) == runThreadId_;
}

void EventLoop::queue_in_loop(std::function<void()>func) {
    {
        std::unique_lock<std::mutex> mtx(tasksMtx_);
        tasks_.push(func);
    }
    uint64_t val = 1;
    write(eventChannel->fd(),&val,sizeof(uint64_t));
}

void EventLoop::heart_beat(){
    itimerspec timeout;
    memset(&timeout,0,sizeof(timeout));
    timeout.it_value.tv_sec = 5;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timeChannel->fd(),0,&timeout,0);
    TimeStamp current = TimeStamp::now();
    std::vector<spConnection> conns;
    for(auto iter:conns_){
        auto connection = iter.second;
        using namespace std::chrono_literals;
        if((current-connection->lastTime())>10s){
            conns.push_back(connection);
        }
    }
    //删除连接
    for(auto conn:conns){
        conns_.erase(conn->fd());
        connectionCloseCb_(conn);
    }
}

void EventLoop::handle_events(){
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