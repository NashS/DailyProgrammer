/*
 * Commons.h
 *
 *  Created on: Jan 21, 2017
 *      Author: beefcake
 *      Always included before the other header files
 */

#ifndef HDR_COMMON_H_
#define HDR_COMMON_H_

#define BROADCAST_CMD "BROADCAST "
#define MAX_BROADCAST_INPUT 100
#define MAX_RECV_INPUT 100
#define PORT "3490"

int recvtimeout(int s, char *buf, int len, int timeout);

#endif /* HDR_COMMON_H_ */
