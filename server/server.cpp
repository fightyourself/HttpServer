#include "HttpServer.h"

int main(int argc, char *argv[]){
    if( argc!=3){
        printf("usage:./server ip port\n");
        return -1;
    }
    HttpServer server(argv[1],atoi(argv[2]));
    server.start();
}