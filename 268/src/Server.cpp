/*
 * server.c
 *
 *  Created on: Aug 19, 2016
 *      Author: beefcake
 */

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <Common.h>


#include "Server.h"

pthread_mutex_t Server::mutexSDClient;
pthread_mutex_t Server::mutexBroadcastInput;

void *clientThreadStartRoutine(void *in)
{
    struct clientThreadInfo *threadIn = (struct clientThreadInfo *)in;
    char recvBuf[MAX_RECV_INPUT] = {0};
    int recvBytes = 0;
    int sendBytes = 0;
    if(threadIn && threadIn->sd_clients)
    {
        int sd_client = threadIn->sd_clients[threadIn->sd_clientsIdx];
        char *sendBuf = threadIn->sendBufs + threadIn->sendBufIdx*MAX_BROADCAST_INPUT;
        printf("thread %d: starting connection to sd %d\n", threadIn->sd_clientsIdx,sd_client);
        while(1)
        {
            if(sendBuf && strlen(sendBuf) && (sendBytes = send(sd_client, sendBuf,strlen(sendBuf),0) != -1)) //Server wants to broadcast to all clients so send then 0 the buffer
            {
                printf("thread %d: sent %d bytes\n", threadIn->sd_clientsIdx, sendBytes);
                printf("thread %d: sent msg '%s'\n",threadIn->sd_clientsIdx, sendBuf);
                Server::setBroadcastInput((char (*)[MAX_BROADCAST_INPUT])threadIn->sendBufs,threadIn->sendBufIdx,NULL);

            }
            recvBytes = recvtimeout(sd_client,recvBuf,MAX_RECV_INPUT,0);
            if(recvBytes > 0)
            {
                printf("thread %d: received %d bytes\n",threadIn->sd_clientsIdx, recvBytes);
                printf("thread %d: received msg '%s'\n",threadIn->sd_clientsIdx, recvBuf);
                if(!strncmp(recvBuf, BROADCAST_CMD,strlen(BROADCAST_CMD)))
                {
                    while(!Server::setBroadcastAll(threadIn->sd_clients, (char (*)[MAX_BROADCAST_INPUT])threadIn->sendBufs,recvBuf+strlen(BROADCAST_CMD)))
                    {
                        printf("now sleeping\n");
                        sleep(TIMEOUT);
                    }
                }
                memset(recvBuf,0,sizeof(recvBuf));
            }
            else if(recvBytes == 0 || recvBytes == -1)  //Connection closed or error
            {
                printf("thread %d: closing connection to sd %d\n",threadIn->sd_clientsIdx, sd_client);
                Server::setSDClient(threadIn->sd_clients,threadIn->sd_clientsIdx,0);
                close(sd_client);
                break;
            }
        }
    }
    else
    {
        printf("invalid thread input given\n");
    }
    pthread_exit(NULL);

}

int Server::setSDClient(int arr[], int i, int sd_client)
{
    int rc = 0;
    pthread_mutex_lock(&mutexSDClient);
    {
        if(i < MAX_CONCURRENT_CLIENTS)
        {
            arr[i] = sd_client;
            rc =  1;
        }

    }
    pthread_mutex_unlock(&mutexSDClient);
    return rc;
}

int Server::getSDClient(const int arr[], int i)
{
    int rc = 0;
    pthread_mutex_lock(&mutexSDClient);
    {
        if(i < MAX_CONCURRENT_CLIENTS)
        {
            rc = arr[i];
        }
    }
    pthread_mutex_unlock(&mutexSDClient);
    return rc;
}

int Server::setBroadcastInput(char arr[][MAX_BROADCAST_INPUT], int i, char *msg)
{
    int rc = 0;
    pthread_mutex_lock(&mutexBroadcastInput);
    {
        if(i < MAX_CONCURRENT_CLIENTS)
        {
            if(msg && strlen(msg) < MAX_BROADCAST_INPUT)
            {
                memcpy(arr[i],msg,strlen(msg));
            }
            else
            {
                memset(arr[i],0,MAX_BROADCAST_INPUT);
            }
            rc = 1;
        }
    }
    pthread_mutex_unlock(&mutexBroadcastInput);
    return rc;
}

const char * Server::getBroadcastInput(const char arr[][MAX_BROADCAST_INPUT], int i)
{
    const char *msg = NULL;
    pthread_mutex_lock(&mutexBroadcastInput);
    {
        if(i < MAX_CONCURRENT_CLIENTS)
        {
            msg = arr[i];
        }
    }
    pthread_mutex_unlock(&mutexBroadcastInput);
    return msg;
}

int Server::setBroadcastAll(int sd_clients[], char arr[][MAX_BROADCAST_INPUT], char *msg)
{
    int rc = 1, i;
    printf("msg: '%s'\n", msg);
    pthread_mutex_lock(&mutexBroadcastInput);
    {
        if(msg && strlen(msg) < MAX_BROADCAST_INPUT)
        {
            //first pass, ensure that there are no outstanding broadcast msg for any other clients before setting a broadcast msg for all clients
            for(i=0; i<MAX_CONCURRENT_CLIENTS; i++)
            {
                printf("checking arr[%d]='%s'\n",i,arr[i]);
                if(arr && arr[i] && strlen(arr[i]))
                {
                    printf("why here\n");
                    rc=0;
                    break;
                }
            }
            if(rc)
            {
                //second pass for setting each clients' sendBuf with the broadcast msg
                for(i=0; i<MAX_CONCURRENT_CLIENTS; i++)
                {
                    if(Server::getSDClient(sd_clients,i))
                        memcpy(arr[i],msg,strlen(msg));
                }
            }
        }
    }
    pthread_mutex_unlock(&mutexBroadcastInput);
    return rc;
}

Server::Server ()
{
    servInfo = NULL;
    sd_listen = 0;
    isInit = 0;
    memset(clientThreadInfos,0,sizeof (clientThreadInfos) );
    memset(sd_clients,0,sizeof (sd_clients) );
    memset(sendBufs,0,sizeof (sendBufs));

    init ();
}

Server::~Server()
{

}

void Server::init ()
{
    int status = 0;
    struct addrinfo hints;
    memset (&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    printf("init server\n");

    if ((status = getaddrinfo ("localhost", PORT, &hints, &servInfo)) != 0)
    {
        fprintf (stderr, "getaddrinfo err: %s\n", gai_strerror (status));
        return;
    }
    if ((sd_listen = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol)) == -1)
    {
        perror("socket");
        return;
    }
    if (bind(sd_listen, servInfo->ai_addr, servInfo->ai_addrlen) == -1)
    {
        perror("bind");
        return;
    }
    if (listen(sd_listen, BACKLOG))
    {
        perror("listen");
        return;
    }

    freeaddrinfo(servInfo);
    isInit = 1;
    return;
}

void Server::run()
{
    int i, sd_tmp;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    pthread_mutex_init(&mutexSDClient, NULL);
    pthread_mutex_init(&mutexBroadcastInput, NULL);

    printf("running server\n");

    while(1)
    {
        if( (sd_tmp = accept(sd_listen,(struct sockaddr *)&their_addr,&sin_size) ) )
        {
            fcntl(sd_tmp, F_SETFL, O_NONBLOCK);
            printf("received client connection\n");
            for(i = 0; i < MAX_CONCURRENT_CLIENTS; i++)
            {
                printf("checking %d\n",i);
                if(!getSDClient(sd_clients,i))
                {
                    printf("using sd_clients[%d]=%d\n",i,sd_tmp);
                    setSDClient(sd_clients,i,sd_tmp);
                    break;
                }
            }
            if(i >= MAX_CONCURRENT_CLIENTS)
            {
                perror("No available sd storage");
                continue;
            }
            clientThreadInfos[i].sd_clients = sd_clients;
            clientThreadInfos[i].sd_clientsIdx = i;
            clientThreadInfos[i].sendBufs = (char *)sendBufs;
            clientThreadInfos[i].sendBufIdx = i;
            if(pthread_create(&threads[i],NULL,clientThreadStartRoutine,(void *)&clientThreadInfos[i]))
            {
                perror("could not create client thread");
                return;
            }
        }
    }

    pthread_mutex_destroy(&mutexSDClient);
    pthread_mutex_destroy(&mutexBroadcastInput);
}

void Server::heartbeat(std::string msg)
{

}

void Server::printIPs()
{
    if(isInit)
    {
        addrinfo *p = NULL;
        void *pAddr = NULL;
        char sIPSTR[INET6_ADDRSTRLEN] = {0};
        char sIPVER[5] = {0};
        for (p = servInfo; p; p = p->ai_next)
        {
            if(AF_INET == p->ai_family)
            {
                pAddr = &((sockaddr_in *)p->ai_addr)->sin_addr;
                strncpy( sIPVER, "IPv4", sizeof sIPVER);
            }
            else if(AF_INET6 == p->ai_family)
            {
                pAddr = &((sockaddr_in6 *)p->ai_addr)->sin6_addr;
                strncpy( sIPVER, "IPv6", sizeof sIPVER);
            }
            else
            {
                continue;
            }

            if ( !inet_ntop(p->ai_family,pAddr,sIPSTR,sizeof sIPSTR) )
            {
                perror("inet_ntop");
            }
            else
            {
                printf("%s: %s\n", sIPVER, sIPSTR);
            }
        }

    }
    else
    {
        printf("Brah, we don't have any socket addresses\n");
    }
}
