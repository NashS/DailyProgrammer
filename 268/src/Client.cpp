/*
 * client.c
 *
 *  Created on: Aug 19, 2016
 *      Author: beefcake
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <Common.h>

#include "Client.h"

Client::Client()
{
    sd_server = 0;
    isInit = 0;
    sendBytes = 0;
    recvBytes = 0;
    servInfo = NULL;
    memset(sendBuf, 0, sizeof(sendBuf));
    memset(recvBuf, 0, sizeof(recvBuf));
}

Client::Client(const char* str)
{
    sd_server = 0;
    isInit = 0;
    sendBytes = 0;
    recvBytes = 0;
    servInfo = NULL;
    memcpy(sendBuf, str, strlen(str));
    memset(recvBuf, 0, sizeof(recvBuf));
}

Client::~Client()
{
    freeaddrinfo(servInfo);
    if(sd_server)
    {
        close(sd_server);
    }
}

void Client::init()
{
    int status = 0;
    struct addrinfo hints;
    memset (&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    printf("init client\n");

    if ((status = getaddrinfo ("localhost", PORT, &hints, &servInfo)) != 0)
    {
        fprintf (stderr, "getaddrinfo err: %s\n", gai_strerror (status));
        return;
    }
    if ((sd_server = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol)) == -1)
    {
        perror("socket");
        return;
    }
    isInit = 1;
    return;
}

void Client::run()
{
    if(isInit)
    {
        int time = 0;
        printf("running client\n");

        if(connect(sd_server,servInfo->ai_addr,servInfo->ai_addrlen) == -1)
        {
            close(sd_server);
            perror("connect");
            return;
        }
        printf("connected to sd_server: %d\n", sd_server);
        while(time < 15)
        {
            if( (recvBytes = recvtimeout(sd_server,recvBuf,MAX_RECV_INPUT,0)) > 0 )
            {
                printf("client: recv %d bytes\n", recvBytes);
                printf("client: recv '%s'\n", recvBuf);
            }
            if(time > 0 || !strlen(sendBuf)) //This will prevent send msg from being "TEST 0" and sending client prog arg instead
                snprintf(sendBuf,sizeof(sendBuf),"TEST %d",time);

            if(strlen(sendBuf) && (sendBytes = send(sd_server,sendBuf,strlen(sendBuf),0)) != -1)
            {
                printf("client: sent %d bytes\n",sendBytes);
                printf("client: sent '%s'\n",sendBuf);
            }
            memset(sendBuf,0, sizeof(sendBuf));
            time++;
            sleep(1);

        }
        close(sd_server);
        printf("closed connection\n");
    }
}
