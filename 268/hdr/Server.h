/*
 * Server.h
 *
 *  Created on: Aug 20, 2016
 *      Author: beefcake
 */

#ifndef HDR_SERVER_H_
#define HDR_SERVER_H_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>

#define BACKLOG 10
#define TIMEOUT 1
#define MAX_CONCURRENT_CLIENTS 5



struct clientThreadInfo{
    int *sd_clients;
    int sd_clientsIdx;
    char *sendBufs;
    int sendBufIdx;
};

class Server
{
private:
    struct addrinfo *servInfo;
    int sd_listen, isInit;

    struct clientThreadInfo clientThreadInfos[MAX_CONCURRENT_CLIENTS];
    int sd_clients[MAX_CONCURRENT_CLIENTS];
    pthread_t threads[MAX_CONCURRENT_CLIENTS];
    char sendBufs[MAX_CONCURRENT_CLIENTS][MAX_BROADCAST_INPUT];

    static pthread_mutex_t mutexSDClient;  //static since used in setSDClient
    static pthread_mutex_t mutexBroadcastInput;

public:
    Server();
    ~Server();
	void init();
	void run();
	void heartbeat(std::string);
	void printIPs();

    static int setSDClient(int arr[], int i, int sd_client);
    static int getSDClient(const int arr[], int i);
    static int setBroadcastInput(char arr[][MAX_BROADCAST_INPUT], int i, char *msg);
    static const char *getBroadcastInput(const char arr[][MAX_BROADCAST_INPUT], int i);
    static int setBroadcastAll(int sd_clients[], char arr[][MAX_BROADCAST_INPUT], char *msg);

};



#endif /* HDR_SERVER_H_ */
