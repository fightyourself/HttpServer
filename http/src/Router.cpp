#include "Router.h"

Router::Router(){

}

Router::~Router(){

}

std::vector<std::string> Router::split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens; // 存放分割后的子字符串
    std::istringstream tokenStream(str);
    std::string token;

    while (std::getline(tokenStream, token, delimiter)) {
        if(token.size()!=0)tokens.push_back(token);
    }

    return tokens; // 返回分割结果的向量
}

void Router::addRoute(const std::string& method,const std::string& pattern,std::function<void(HttpRequest *,spConnection)> handler){
    if(roots_.find(method)==roots_.end()){
        roots_[method] = new Trie;
    }
    Trie * root = roots_[method];
    std::vector<std::string> parts = split(pattern,'/');
    root->insert(pattern,parts,0);
    std::string key = method+"-"+pattern;
    handlers_[key] = handler;
}


void Router::GET(const std::string& pattern,std::function<void(HttpRequest *,spConnection)> handler){
    this->addRoute("GET",pattern,handler);
}

void Router::POST(const std::string& pattern,std::function<void(HttpRequest *,spConnection)> handler){
    this->addRoute("POST",pattern,handler);
}

void Router::handle(HttpRequest *req,spConnection conn){
    std::vector<std::string> parts = split(req->path(),'/');
    std::string method = req->method();
    if(roots_.find(method)!=roots_.end()){
        Trie * root = roots_[method];
        Trie * node = root->search(parts,0);
        if(node!=nullptr){
            std::string key = method + "-" + req->path();
            handlers_[key](req,conn);
            return;
        }
    }
    HttpResponse resp(404);
    resp.set_header("Content-Type","text/html");
    std::string html = R"(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>404 - 页面未找到</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f5f5f5;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .error-container {
            text-align: center;
            padding: 2rem;
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }
        h1 {
            font-size: 4rem;
            color: #e74c3c;
            margin: 0;
        }
        p {
            font-size: 1.2rem;
            color: #333;
            margin: 1rem 0;
        }
        a {
            color: #3498db;
            text-decoration: none;
            font-weight: bold;
        }
        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="error-container">
        <h1>404</h1>
        <p>抱歉，您访问的页面不存在。</p>
        <p>请检查 URL 地址是否正确，或点击 <a href="/">返回首页</a>。</p>
    </div>
</body>
</html>
)";
    resp.set_body(html);
    conn->send(resp.to_string());
}