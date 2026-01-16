#pragma once
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

class EventLoop;
class Channel;

class Connection{
private:
    EventLoop *loop_;
    Socket *clientSock_;
    Channel *clientChan_;
    Buffer inputBuf_;
    Buffer outputBuf_;
    std::function<void()> connectionCloseCb_;
    std::function<void()> connectionErrorCb_;
    std::function<void(Connection*,std::string&)> onMessageCb_;
    std::function<void(Connection*)> sendOverCb_;
public:
    Connection(Socket *clientSock,EventLoop *loop);
    ~Connection();
    std::string ip()const;
    uint16_t port() const;
    int fd() const;
    void connection_close();
    void connection_error();
    void on_message();
    void send(char *data,int len);
    void send_all_data();

    void set_connection_close_cb(std::function<void()>func);
    void set_connection_error_cb(std::function<void()>func);
    void set_on_message_cb(std::function<void(Connection *,std::string &)>func);
    void set_send_over_cb(std::function<void(Connection*)>func);
};