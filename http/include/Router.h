#pragma once
#include "Trie.h"
#include "HttpRequest.h"
#include "Connection.h"
#include "HttpResponse.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <sstream>
class Router{
private:
    std::unordered_map<std::string,Trie *>roots_;
    std::unordered_map<std::string,std::function<void(HttpRequest *req,Connection *conn)>> handlers_;
    std::vector<std::string> split(const std::string& pattern, char delimiter);
    void addRoute(const std::string& method,const std::string& pattern,std::function<void(HttpRequest *req,Connection *conn)> handler);
public:
    Router();
    ~Router();
    void GET(const std::string& pattern,std::function<void(HttpRequest *req,Connection *conn)> handler);
    void POST(const std::string& pattern,std::function<void(HttpRequest *req,Connection *conn)> handler);
    void handle(HttpRequest *req,Connection *conn);
};

