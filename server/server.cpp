//
// Created by hd on 2022/9/3.
//

#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "../common_define/const_define.h"


void startServer() {
    int listenFd,conFd;
    struct sockaddr_in servAddress;
    char buff[MAX_LINE];
    time_t ticks;

    listenFd = socket(AF_INET,SOCK_STREAM,0);
    if (listenFd < 0) {
        std::cout << "create socket error: " << listenFd << std::endl;
    }
    bzero(&servAddress,sizeof(servAddress));

    servAddress.sin_family = AF_INET;
    servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddress.sin_port = htons(8899);

    int ret = bind(listenFd,(const struct sockaddr *)&servAddress,sizeof(servAddress));
    if (ret < 0) {
        std::cout << "bind error: " << ret << std::endl;
    }

    ret = listen(listenFd,1000);
    if (ret < 0) {
        std::cout << "listen error: " << ret << std::endl;
    }

    while(true) {
        int connfd = accept(listenFd,(struct sockaddr*) nullptr, nullptr);
        ticks = time(NULL);
        //ctime直接将time返回的时间戳转换为localtime_r然后找对应的char*
        snprintf(buff,sizeof(buff),"%.24s", ctime(&ticks));
        write(connfd,buff,strlen(buff));
        close(connfd);
    }
}


int main() {
    startServer();
    return 0;
}