#pragma once
#include "Buffer.h"
#include "HttpRequest.h"
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
    HttpRequest req_;
    int get_line(char *buf);
public:
    HttpParser();
    ~HttpParser();
    
    HttpRequest req() const;

    void clear_req();
    void append(char *data,size_t size);
    int parse();
    void parse_error();
};