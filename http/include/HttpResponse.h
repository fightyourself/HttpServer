#pragma once
#include <map>
#include <string>

class HttpResponse{
private:
    int statusCode_;
    std::string statusMessage_;
    std::map<std::string, std::string> headers_;
    std::string body_;

    static std::string getDefaultStateMessage(int code){
        switch (code){
            case 200: return "OK";
            case 400: return "Bad Request";
            case 404: return "Not Found";
            case 500: return "Internal Server Error";
            default: return "Unknow Status Code";
        }
    }
public:
    HttpResponse(int code = 200);
    ~HttpResponse();
    
    void set_status(int code, const std::string&msg = "");
    void set_body(const std::string& content);
    void set_header(const std::string &key,const std::string &value);
    std::string to_string() const;
};