#include "Channel.h"
#include <unistd.h>
Channel::Channel(int fd,const Epoll * ep):ep_(ep),fd_(fd){

}

Channel::~Channel(){
}

int Channel::fd() const{
    return fd_;
}

bool Channel::is_inpoll() const {
    return  isInpoll_;
}

uint32_t Channel::events() const {
    return events_;
}

uint32_t Channel::revents() const {
    return revents_;
}

void Channel::set_isInpoll(){
    isInpoll_ = true;
}

void Channel::set_et(){
    events_ |= EPOLLET;
}


void Channel::set_revents(uint32_t ev){
    revents_ = ev;
}

void Channel::set_read_callback(std::function<void()>func){
    readCallback_ = func;
}

void Channel::set_disconnect_cb(std::function<void()>func){
    disconnectCb_ = func;
}

void Channel::set_error_cb(std::function<void()>func){
    errorCb_ = func;
}

void Channel::set_write_cb(std::function<void()>func){
    writeCb_ = func;
}

void Channel::enable_read(){
    events_ |= EPOLLIN;
    ep_->update_channel(this);
}

void Channel::enable_write(){
    events_ |= EPOLLOUT;
    ep_->update_channel(this);
}

void Channel::disable_write(){
    events_ &= ~EPOLLOUT;
    ep_->update_channel(this);
}

void Channel::disalbe_all(){
    events_ = 0;
    ep_->update_channel(this);
}

void Channel::handle(){
    // printf("events(%d):",fd_);
    if(revents_& EPOLLRDHUP){
        // printf("EPOLLRDHUP\n");
        disconnectCb_();
    }else if(revents_ & (EPOLLIN | EPOLLPRI)){
        // printf("EPOLLIN | EPOLLPRI\n");
        readCallback_();
    }else if(revents_ & EPOLLOUT){
        // printf("EPOLLOUT\n");
        writeCb_();
    }else{
        errorCb_();
    }
}
