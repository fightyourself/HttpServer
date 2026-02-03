#pragma once
// #include "Connection.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <sstream>
#include <memory>
class Trie;
class HttpRequest;
class Connection;
using spConnection = std::shared_ptr<Connection>;
class Router{
private:
    std::unordered_map<std::string,Trie *>roots_;
    std::unordered_map<std::string,std::function<void(HttpRequest *,spConnection)>> handlers_;
    std::vector<std::string> split(const std::string& pattern, char delimiter);
    void addRoute(const std::string& method,const std::string& pattern,std::function<void(HttpRequest *,spConnection)> handler);
public:
    Router();
    ~Router();
    void GET(const std::string& pattern,std::function<void(HttpRequest *,spConnection)> handler);
    void POST(const std::string& pattern,std::function<void(HttpRequest *,spConnection)> handler);
    void handle(HttpRequest *req,spConnection);
};

