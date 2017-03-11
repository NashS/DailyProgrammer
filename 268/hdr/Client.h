/*
 * client.h
 *
 *  Created on: Aug 20, 2016
 *      Author: beefcake
 */

#ifndef HDR_CLIENT_H_
#define HDR_CLIENT_H_

class Client{


private:
    struct addrinfo *servInfo;
    char recvBuf[MAX_RECV_INPUT];
    char sendBuf[MAX_BROADCAST_INPUT];
    int sd_server;
    int sendBytes;
    int recvBytes;
    int isInit;

public:
    Client();
    Client(const char* str);
    ~Client();
    void init();
    void run();
};


#endif /* HDR_CLIENT_H_ */
