#include "Buffer.h"


Buffer::Buffer():buf_(){

}

Buffer::~Buffer(){

}

void Buffer::append(const char *data,int len){
    buf_.append(data,len);
}

void Buffer::append_with_len(const char *data, int len){
    append(reinterpret_cast<const char*>(&len),4);
    append(data,len);
}

size_t Buffer::size() const{
    return buf_.size();
}

char * Buffer::data() {
    return buf_.data();
}

void Buffer::clear(){
    buf_.clear();
}

void Buffer::erase(int pos, size_t len){
    buf_.erase(pos,len);
}