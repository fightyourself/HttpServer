#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include "InetAddress.h"
#include "Socket.h"
int main(int argc, char *argv[]){
    if(argc!=3){
        printf("usage:./client ip port\n");return -1;
    }
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd<0){
        printf("create socket fail\n");
        return -1;
    }
    char buf[1024];
    InetAddress clientAddr(argv[1],atoi(argv[2]));
    if(connect(sockfd,clientAddr.addr(),sizeof(sockaddr))!=0){
        printf("connect fail\n");
        close(sockfd);
        return -1;
    }
    printf("connnect ok\n");
    // for(int i=0;i<2;i++){
    //     memset(buf,0,sizeof(buf));
    //     sprintf(buf,"this is %d super girl", i);
    //     int len = strlen(buf);
    //     char tmp[1024];
    //     memcpy(tmp,&len,4);
    //     memcpy(tmp+4,buf,len);
    //     if(send(sockfd,tmp,len+4,0)<=0){
    //         printf("send failed\n");close(sockfd);return -1;
    //     }
    // }
    
    // sleep(100);
    // for(int i=0;i<2;i++){
    //     memset(buf,0,sizeof(buf));
    //     int len;
    //     recv(sockfd,&len,4,0);
    //     recv(sockfd,buf,len,0);
    //     printf("recv:%s\n",buf);
    // }
    std::string txt ="GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: Mozilla/5.0 (compatible; MyClient/1.0)\r\nAccept: text/html,application/xhtml+xml;q=0.9,*/*;q=0.8\r\nConnection: close\r\n\r\n";
    sprintf(buf,"%s",txt.c_str());
    while(true){
        if(send(sockfd,buf,strlen(buf),0)<=0){
            printf("send fail\n");
            return -1;
        }
        sleep(2);
        // memset(buf,0,sizeof(buf));
    
        // if(recv(sockfd,buf,sizeof(buf),0)<=0){
        //     printf("recv fail\n");
        //     return -1;
        // }
        // printf("recv:%s\n",buf);
    }

}