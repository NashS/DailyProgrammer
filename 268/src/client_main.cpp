/*
 * client_main.cpp
 *
 *  Created on: Dec 26, 2016
 *      Author: beefcake
 */

#include <Common.h>
#include <stdio.h>
#include <string.h>

#include "Client.h"

int main (int argc, char * argv[])
{
    if(argc > 1)
    {
        Client client(argv[1]);
        client.init();
        client.run();
    }
    else
    {
        Client client;
        client.init();
        client.run();
    }

    return 0;
}
