#include "../include/HttpRequest.h"
#include <cstring>
HttpRequest::HttpRequest(){

}


HttpRequest::~HttpRequest(){

}

const char * HttpRequest::method() const{
    return method_;
}

const char * HttpRequest::path() const{
    return path_;
}

const char * HttpRequest::version() const{
    return version_;
}

std::string HttpRequest::body() const{
    return body_;
}

size_t HttpRequest::content_length()const{
    return contentLength_;
}

std::map<std::string,std::string> HttpRequest::headers() {
    return headers_;
}



void HttpRequest::add_header(const std::string &key, const std::string &value){
    headers_[key] = value;
}

void HttpRequest::set_content_length(size_t content_size){
    contentLength_ = content_size;
}

void HttpRequest::set_has_body(){
    hasBody_ = true;
}

void HttpRequest::set_method(const char * method){
    strcpy(method_,method);
}

void HttpRequest::set_path(const char * path){
    strcpy(path_,path);
}

void HttpRequest::set_version(const char * version){
    strcpy(version_,version);
}

void HttpRequest::set_body(const std::string &body){
    body_ = body;
}