#pragma once
#include "../include/Buffer.h"
#include "../include/HttpRequest.h"
enum ParseState{
    REQUEST_LINE,
    REQUEST_HEADER,
    REQUEST_BODY,
    COMPLETE,
    ERROR
};

class HttpParser{
private:
    Buffer buf_;
    size_t pos_;
    ParseState state_;
    int get_line(char *buf);
public:
    HttpParser();
    ~HttpParser();
    
    void append(char *data,size_t size);
    int parse(HttpRequest *);
    void parse_error();
};