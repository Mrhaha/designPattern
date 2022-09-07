//
// Created by hd on 2022/9/3.
//

#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "../common_define/const_define.h"


void startConnect() {
    int sendFd;
    struct sockaddr_in servAddress;
    char buff[MAX_LINE];
    time_t ticks;

    sendFd = socket(AF_INET,SOCK_STREAM,0);
    if (sendFd < 0) {
        std::cout << "create socket error: " << sendFd << std::endl;
    }
    bzero(&servAddress,sizeof(servAddress));

    servAddress.sin_family = AF_INET;
    servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddress.sin_port = htons(8899);

    int ret = connect(sendFd,(struct sockaddr*)&servAddress,sizeof(servAddress));
    if (ret < 0) {
        std::cout << "connect socket error: " << ret << std::endl;
    }
    ret = read(sendFd,buff,sizeof(buff));
    std::cout << "read: " << ret << "byte" << std::endl;
    std::cout << "readBuff: " << buff << std::endl;
}


int main() {
    startConnect();
    return 0;
}