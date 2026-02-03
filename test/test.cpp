#include "EventLoop.h"
#include "ThreadPool.h"
#include "Connection.h"
#include "HttpRequest.h"
#include <iostream>
int main(){
    std::cout<<sizeof(Connection)<<std::endl;
    return 0;
}