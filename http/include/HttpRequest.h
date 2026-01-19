#pragma once
#include <map>
#include <string>

class HttpRequest{
private:
    std::string method_;
    std::string path_;
    std::string version_;
    std::map<std::string,std::string> headers_;
    size_t contentLength_ = 0;
    bool hasBody_ = false;
    std::string body_;
    std::string queryString_;
public:
    HttpRequest();
    ~HttpRequest();

    const std::string & method() const;
    const std::string & path() const;
    const std::string & version() const;
    std::string body() const;
    size_t content_length() const;
    std::string queryString() const;

    std::map<std::string,std::string> headers();

    void set_method(const char *method);
    void set_path(const char *path);
    void set_version(const char *version);
    void add_header(const std::string &key,const std::string &value);
    void set_content_length(size_t content_size);
    void set_has_body();
    void set_body(const std::string& body);
    void set_query_string(const std::string& queryString);
};