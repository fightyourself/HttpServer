// #include "../include/HttpParser.h"
// #include "../include/HttpRequest.h"
// #include <string>
// int main(){
//     HttpParser paser;
//     HttpRequest req;
//     std::string txt ="GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: Mozilla/5.0 (compatible; MyClient/1.0)\r\nAccept: text/html,application/xhtml+xml;q=0.9,*/*;q=0.8\r\nConnection: close\r\n\r\n";
//     for(int i=0;i<100;i++){
//         paser.append(txt.data(),txt.size());
//     }
//     while(paser.parse(&req)){
//         printf("method = %s,path = %s, version = %s\n",req.method(),req.path(),req.version());
//     }
//     return 0;
// }

#include "../include/HttpServer.h"

int main(int argc, char *argv[]){
    HttpServer server("127.0.0.1",5005);
    server.start();
}