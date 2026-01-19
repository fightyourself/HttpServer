#include "../include/HttpResponse.h"
#include <sstream>

HttpResponse::HttpResponse(int code):statusCode_(code),statusMessage_(getDefaultStateMessage(code)){

}

HttpResponse::~HttpResponse(){

}

void HttpResponse::set_status(int code,const std::string &msg){
    statusCode_ = code;
    if(msg.empty()){
        statusMessage_ = getDefaultStateMessage(statusCode_);
    }else {
        statusMessage_ = msg;
    }
}


void HttpResponse::set_body(const std::string& content){
    body_ = content;
    set_header("Content-Length",std::to_string(body_.size()));
}

void HttpResponse::set_header(const std::string&key, const std::string& value){
    headers_[key] = value;
}

std::string HttpResponse::to_string() const {
    std::stringstream ss;
    ss << "HTTP/1.1 " << statusCode_ <<" "<<statusMessage_<<"\r\n";
    for(auto &header:headers_){
        ss<<header.first<<": "<<header.second<<"\r\n";
    }
    if (headers_.find("Content-Length") == headers_.end()) {
        ss << "Content-Length: " << body_.size() << "\r\n";
    }
    ss<<"\r\n";
    ss<<body_;
    return ss.str();
}
