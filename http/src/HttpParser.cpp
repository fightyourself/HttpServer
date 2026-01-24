#include "../include/HttpParser.h"
#include <cstring>
HttpParser::HttpParser():buf_(),pos_(0),state_(REQUEST_LINE){

}

HttpParser::~HttpParser(){
    
}

int HttpParser::get_line(char *buf){
    char * data = buf_.data();
    size_t count = 0;
    size_t i = pos_;
    while(i<buf_.size()){
        buf[count++] = data[i];
        if(data[i]=='\r'){
            if(i+1<buf_.size() && data[i+1]=='\n'){
                buf[count++] = data[i+1];
                buf[count] = '\0';
                pos_ = i+2;
                return count;
            }
        }else ++i;
    }
    return 0;
}


int HttpParser::parse(HttpRequest * req){
    while(true){
        char line[2048];
        switch (state_){
        case REQUEST_LINE:{
            if(get_line(line)){
                size_t i = 0;
                size_t j = 0;
                char method[16];
                while(line[i] && !isspace(line[i]))i++;
                size_t len = i - j;
                if(len>15){
                    parse_error();
                    break;
                }
                memcpy(method,reinterpret_cast<void*>(&line[j]),len);
                method[len] = '\0';
                if(strcasecmp(method,"GET") && strcasecmp(method,"POST")){
                    parse_error();
                    break;
                }
                // path
                while(line[i] && isspace(line[i])) i++;
                j = i;
                while(line[i] && !isspace(line[i])) i++;
                if(strcasecmp(method,"GET")==0){
                    size_t t = j;
                    while(line[t] && line[t]!='?')++t;
                    if(line[t]=='?')req->set_query_string(std::string(&line[t+1],i-t));
                }
                char path[1024];
                len = i - j;
                memcpy(path,reinterpret_cast<void*>(&line[j]),len);
                path[len] = '\0';
                //version
                while(line[i] && isspace(line[i])) i++;
                j = i;
                while(line[i]!='\r') i++;
                char version[16];
                len = i -j;
                memcpy(version,reinterpret_cast<void*>(&line[j]),len);
                version[len] = '\0';

                req->set_method(method);
                req->set_path(path);
                req->set_version(version);
                state_ = REQUEST_HEADER;
            }else return 0;
            break;
        }
        case REQUEST_HEADER:{
            while(get_line(line)){
                if(strcmp(line,"\r\n")==0){
                    if(req->headers().find("Content-Length")!=req->headers().end()){
                        req->set_content_length(atoi(req->headers()["Content-Length"].c_str()));
                    }
                    if(req->content_length() <=0) state_ = COMPLETE;
                    else state_ = REQUEST_BODY;
                    break;
                }
                size_t i = 0;
                size_t j = 0;
                while(line[i]&&line[i]!=':')i++;
                std::string key(&line[j],i-j);
                i+=2;
                j = i;
                while(line[i]!='\r')i++;
                std::string value(&line[j],i-j);
                req->add_header(key,value);
            }
            if(state_ != REQUEST_HEADER) break;
            return 0;
        }
        case REQUEST_BODY:{
            if(buf_.size()-pos_>=req->content_length()){
                req->set_has_body();
                req->set_body(std::string(&(buf_.data()[pos_]),req->content_length()));
                state_ = COMPLETE;
                break;
            }
            return 0;
        }
        case COMPLETE:{
            buf_.erase(0,pos_);
            pos_ = 0;
            state_ = REQUEST_LINE;
            return 1;
        }
        default:
            break;
        }
    }
}

void HttpParser::parse_error(){
    state_ = ERROR;
    printf("parse error\n");
}


void HttpParser::append(char *data, size_t size){
    buf_.append(data,size);
}