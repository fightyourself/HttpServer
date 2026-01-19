#include "../include/Connection.h"

Connection::Connection(Socket *clientSock,EventLoop *loop):clientSock_(clientSock),loop_(loop){
    clientChan_ = new Channel(clientSock->fd(),loop_);
    clientChan_->set_et();
    clientChan_->set_read_callback(std::bind(&Connection::connection_read,this));
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

HttpParser & Connection::parser(){
    return parser_;
}

Buffer & Connection::ouputBuf(){
    return outputBuf_;
}


void Connection::set_connection_close_cb(std::function<void()>func){
    connectionCloseCb_ = func;
}

void Connection::set_connection_error_cb(std::function<void()>func){
    connectionErrorCb_ = func;
}

void Connection::set_connection_read_cb(std::function<void(Connection *)>func){
    connectionReadCb_ = func;
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

void Connection::connection_read(){
    char c;
    if(recv(fd(),&c,1,MSG_PEEK)==0){
        connection_close();
    }else{
        connectionReadCb_(this);
    }
    
}


void Connection::send(const char *data,int len){
    outputBuf_.append(data,len);
    clientChan_->enable_write();
}

void Connection::send(const std::string&data){
    send(data.data(),data.size());
}

void Connection::send_all_data(){
    int writen = ::send(fd(),outputBuf_.data(),outputBuf_.size(),0);
    if(writen>0) outputBuf_.erase(0,writen);
    if(outputBuf_.size()==0) {
        clientChan_->disable_write();
        sendOverCb_(this);
    }
}
