/*
 * server.c
 *
 *  Created on: Aug 19, 2016
 *      Author: beefcake
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT "3490"

#include <pthread.h>
#include <semaphore.h>

#include "Server.h"

Server::Server ()
{
    this->init ();
}

void
Server::init ()
{
    int status;
    struct addrinfo hints;
    memset (&hints, 0, sizeof hints);
    struct addrinfo *servInfo = NULL;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo (NULL, PORT, &hints, &servInfo)) != 0)
    {
        fprintf (stderr, "getaddrinfo err: %s\n", gai_strerror (status));
    }
}
