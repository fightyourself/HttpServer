#include "Connection.h"
#include "Channel.h"
#include "Socket.h"
#include "EventLoop.h"
Connection::Connection(std::unique_ptr<Socket> clientSock,EventLoop *loop):loop_(loop),clientSock_(std::move(clientSock)),
clientChan_(new Channel(clientSock_->fd(),loop_->ep())),parser_(),outputBuf_(),isClosed_(false),lastTime_(),connectionCloseCb_(),
connectionErrorCb_(),connectionReadCb_(),sendOverCb_(){
    clientChan_->set_et();
    clientChan_->set_read_callback(std::bind(&Connection::connection_read,this));
    clientChan_->set_disconnect_cb(std::bind(&Connection::connection_close,this));
    clientChan_->set_error_cb(std::bind(&Connection::connection_error,this));
    clientChan_->set_write_cb(std::bind(&Connection::send_all_data,this));
    clientChan_->enable_read();
}

Connection::~Connection(){
    // printf("connection destruction\n");
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

EventLoop* Connection::loop() const{
    return loop_;
}

TimeStamp Connection::lastTime() const{
    return lastTime_;
}

HttpParser & Connection::parser(){
    return parser_;
}

Buffer & Connection::ouputBuf(){
    return outputBuf_;
}

void Connection::remove_channel_from_loop(){
    loop_->ep()->remove_channel(clientChan_.get());
}

void Connection::set_is_closed() {
    isClosed_ = true;
}

void Connection::update_time_stamp() {
    lastTime_ = TimeStamp::now();
}

void Connection::set_connection_close_cb(std::function<void(spConnection)>func){
    connectionCloseCb_ = func;
}

void Connection::set_connection_error_cb(std::function<void(spConnection)>func){
    connectionErrorCb_ = func;
}

void Connection::set_connection_read_cb(std::function<void(spConnection)>func){
    connectionReadCb_ = func;
}

void Connection::set_send_over_cb(std::function<void(spConnection)> func){
    sendOverCb_ = func;
}


void Connection::connection_close(){
    connectionCloseCb_(shared_from_this()); 
}

void Connection::connection_error(){
    connectionErrorCb_(shared_from_this());
}

void Connection::connection_read(){
    connectionReadCb_(shared_from_this());
}

void Connection::send_init(const std::string&data){
    // printf("Connectin::send_init() in thread:%d\n",syscall(SYS_gettid));
    outputBuf_.append(data.data(),data.size());
    clientChan_->enable_write();
}



void Connection::send(const std::string&data){
    if(isClosed_) {
        // printf("connection already closed, no send\n");
        return;
    }
    if(loop_->isInRunThread()){
        // printf("Connection::send() in run thread:%d\n",syscall(SYS_gettid));
        send_init(data);
    }else{
        // printf("Connection::send() not in run thread:%d\n",syscall(SYS_gettid));
        loop_->queue_in_loop(std::bind(&Connection::send_init,this,data));
    }
}

void Connection::send_all_data(){
    int writen = ::send(fd(),outputBuf_.data(),outputBuf_.size(),0);
    if(writen>0) outputBuf_.erase(0,writen);
    if(outputBuf_.size()==0) {
        clientChan_->disable_write();
        sendOverCb_(shared_from_this());
    }
}
