#pragma once
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "../include/HttpParser.h"
#include "../include/HttpRequest.h"
#include <memory>
#include <atomic>

class EventLoop;
class Channel;
class Connection;
using spConnection = std::shared_ptr<Connection>;
class Connection:public std::enable_shared_from_this<Connection> {
private:
    EventLoop *loop_;
    std::unique_ptr<Socket> clientSock_;
    std::unique_ptr<Channel> clientChan_;
    HttpParser parser_;
    Buffer outputBuf_;
    std::atomic_bool isClosed_;
    std::function<void(spConnection)> connectionCloseCb_;
    std::function<void(spConnection)> connectionErrorCb_;
    std::function<void(spConnection)> connectionReadCb_;
    std::function<void(spConnection)> sendOverCb_;
    void send_init(const std::string &data);
public:
    Connection(std::unique_ptr<Socket> clientSock,EventLoop *loop);
    ~Connection();
    std::string ip()const;
    uint16_t port() const;
    int fd() const;
    HttpParser &parser();
    Buffer & ouputBuf();

    void remove_channel_from_loop();
    void set_is_closed();

    void send(const std::string &data);
    void send_all_data();

    void connection_close();
    void connection_error();
    void connection_read();

    void set_connection_close_cb(std::function<void(spConnection)>func);
    void set_connection_error_cb(std::function<void(spConnection)>func);
    void set_connection_read_cb(std::function<void(spConnection)>func);
    void set_send_over_cb(std::function<void(spConnection)>func);
};