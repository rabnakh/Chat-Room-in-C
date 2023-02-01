#ifndef SERVER_DRIVER_H
#define SERVER_DRIVER_H

#include "serverLoginMenu.h"
#include "serverInnerMenu.h"

void *serverDriver(void *arg){
	int sockfd = (long) arg;
	int userline;
	while(1){
		userline = serverLoginMenu(sockfd);
		serverInnerMenu(sockfd,userline);
	}
}

#endif
