#include "Connection.h"

Connection::Connection(Socket *clientSock,EventLoop *loop):clientSock_(clientSock),loop_(loop){
    clientChan_ = new Channel(clientSock->fd(),loop_);
    clientChan_->set_et();
    clientChan_->set_read_callback(std::bind(&Connection::on_message,this));
    clientChan_->set_disconnect_cb(std::bind(&Connection::connection_close,this));
    clientChan_->set_error_cb(std::bind(&Connection::connection_error,this));
    clientChan_->set_write_cb(std::bind(&Connection::send_all_data,this));
    clientChan_->enable_read();
}

Connection::~Connection(){
    delete clientSock_;
    delete clientChan_;
}

std::string Connection::ip()const{
    return clientSock_->ip();
}

uint16_t Connection::port() const{
    return clientSock_->port();
}

int Connection::fd() const {
    return clientSock_->fd();
}

void Connection::set_connection_close_cb(std::function<void()>func){
    connectionCloseCb_ = func;
}

void Connection::set_connection_error_cb(std::function<void()>func){
    connectionErrorCb_ = func;
}

void Connection::set_on_message_cb(std::function<void(Connection*,std::string&)>func){
    onMessageCb_ = func;
}

void Connection::set_send_over_cb(std::function<void(Connection *)> func){
    sendOverCb_ = func;
}


void Connection::connection_close(){
    connectionCloseCb_();
}

void Connection::connection_error(){
    connectionErrorCb_();
}

#include <sys/syscall.h>
void Connection::on_message(){
    printf("on message event %ld\n",syscall(SYS_gettid));
    char buf[1024];
    while(true){
        bzero(&buf,sizeof(buf));
        int readn = recv(fd(),buf,sizeof(buf),0);
        if(readn>0){
            inputBuf_.append(buf,readn);
        }else if(readn==-1 && errno == EINTR){
            continue;
        }else if(readn==-1 && (errno==EAGAIN||errno == EWOULDBLOCK)){
            while(true){
                if(inputBuf_.size()<4) break;
                int len;
                memcpy(&len,inputBuf_.data(),4);
                if(inputBuf_.size()<len+4)break;
                std::string message(inputBuf_.data()+4,len);
                inputBuf_.erase(0,len+4);
                onMessageCb_(this,message);
            }
            break;
        }else if(readn==0){
            connection_close();
            break;
        }
    }
}


void Connection::send(char *data,int len){
    outputBuf_.append_with_len(data,len);
    clientChan_->enable_write();
}

void Connection::send_all_data(){
    int writen = ::send(fd(),outputBuf_.data(),outputBuf_.size(),0);
    if(writen>0) outputBuf_.erase(0,writen);
    if(outputBuf_.size()==0) {
        clientChan_->disable_write();
        sendOverCb_(this);
    }
}
