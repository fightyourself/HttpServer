#include "Epoll.h"

Epoll::Epoll():epollFd_(epoll_create(1)),events_(){

}

Epoll::~Epoll(){

}

void Epoll::update_channel(Channel *ch) const{
    epoll_event ev; 
    ev.data.ptr = reinterpret_cast<void *>(ch);
    ev.events = ch->events();
    if(ch->is_inpoll()){
        epoll_ctl(epollFd_,EPOLL_CTL_MOD,ch->fd(),&ev);
    }else{
        epoll_ctl(epollFd_,EPOLL_CTL_ADD,ch->fd(),&ev);
        ch->set_isInpoll();
    }
}

void Epoll::remove_channel(Channel *ch)const{
    ch->disalbe_all();
    if(ch->is_inpoll()){
        epoll_ctl(epollFd_,EPOLL_CTL_DEL,ch->fd(),0);
    }
}

#include <sys/syscall.h>
std::vector<Channel *> Epoll::loop(int timeout){
    std::vector<Channel *> evs;
    bzero(events_,sizeof(events_));
    int infds =epoll_wait(epollFd_,events_,MaxElement,timeout);
    if(infds<0){
        printf("[%d]%s:%s:%d:%d\n",syscall(SYS_gettid),__FILE__,__FUNCTION__,__LINE__,errno);
        exit(-1);
    }
    if(infds==0){
        return evs;
    }
    for(int i = 0;i<infds;i++){
        Channel *ch = reinterpret_cast<Channel *>(events_[i].data.ptr);
        ch->set_revents(events_[i].events);
        evs.push_back(ch);
    }
    return evs;
}