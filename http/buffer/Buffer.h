#pragma once
#include <string>
class Buffer{
private:
    std::string buf_;
public:
    Buffer();
    ~Buffer();

    void append(const char *data,int len);
    void append_with_len(const char *data, int len);
    size_t size() const;
    const char * data();
    void clear();
    void erase(int pos,size_t len);
};