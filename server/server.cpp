#include "HttpServer.h"

int main(int argc, char *argv[]){
    if( argc!=3){
        printf("usage:./server ip port\n");
        return -1;
    }
    HttpServer server(argv[1],atoi(argv[2]));
    server.GET("/",[](HttpRequest *req,spConnection conn){
        HttpResponse resp;
        std::string html =
            "<!DOCTYPE html>\r\n"
            "<html>\r\n"
            "<head>\r\n"
            "    <title>Hello World</title>\r\n"
            "</head>\r\n"
            "<body>\r\n"
            "    <h1>Welcome to My Simple Page!</h1>\r\n"
            "    <p>This is a basic HTML page served via HTTP.</p>\r\n"
            "</body>\r\n"
            "</html>\r\n";
        resp.set_header("Content-Type","text/html");
        resp.set_body(html);
        conn->send(resp.to_string());
    });

    server.start();
}