#include <sstream>
#include <vector>
#include <string>
#include <iostream>
std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens; // 存放分割后的子字符串
    std::istringstream tokenStream(str);
    std::string token;

    while (std::getline(tokenStream, token, delimiter)) {
        if(token.size()!=0)tokens.push_back(token);
    }

    return tokens; // 返回分割结果的向量
}

int main(){
    
}