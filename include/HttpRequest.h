#pragma once
#include <map>
#include <string>

class HttpRequest{
private:
    char method_[16];
    char path_[1024];
    char version_[16];
    std::map<std::string,std::string> headers_;
    size_t contentLength_ = 0;
    bool hasBody_ = false;
    std::string body_;
    std::string queryString_;
public:
    HttpRequest();
    ~HttpRequest();

    const char * method() const;
    const char * path() const;
    const char * version() const;
    std::string body() const;
    size_t content_length() const;

    std::map<std::string,std::string> headers();

    void set_method(const char *method);
    void set_path(const char *path);
    void set_version(const char *version);
    void add_header(const std::string &key,const std::string &value);
    void set_content_length(size_t content_size);
    void set_has_body();
    void set_body(const std::string& body);
};