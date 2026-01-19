#include "../include/HttpRequest.h"
#include <cstring>
HttpRequest::HttpRequest(){

}


HttpRequest::~HttpRequest(){

}

const std::string & HttpRequest::method() const{
    return method_;
}

const std::string & HttpRequest::path() const{
    return path_;
}

const std::string & HttpRequest::version() const{
    return version_;
}

std::string HttpRequest::body() const{
    return body_;
}

size_t HttpRequest::content_length()const{
    return contentLength_;
}

std::string HttpRequest::queryString() const{
    return queryString_;
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
    method_.append(method);
}

void HttpRequest::set_path(const char * path){
    path_.append(path);
}

void HttpRequest::set_version(const char * version){
    version_.append(version);
}

void HttpRequest::set_body(const std::string &body){
    body_ = body;
}

void HttpRequest::set_query_string(const std::string &queryString){
    queryString_ = queryString;
}