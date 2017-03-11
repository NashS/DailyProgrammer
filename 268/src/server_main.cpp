/*
 * test_easy.c
 *
 *  Created on: Aug 19, 2016
 *      Author: beefcake
 */

#include <Common.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "Server.h"

int main (int argc, char * argv[])
{
    Server serv;
    serv.printIPs();
    serv.run();

    return 0;
}
