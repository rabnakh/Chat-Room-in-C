#ifndef SERVER_DRIVER_H
#define SERVER_DRIVER_H

#define EXIT_CLEAN 0
#define EXIT_ESC 1
#define EXIT_CTRL_C 2

#include "serverLoginMenu.h"
#include "serverInnerMenu.h"

void *serverDriver(void *arg){
	int err;
	int sockfd = (long) arg;
	char username[11];
	while(1){
		err = serverLoginMenu(sockfd,username);
		if(err == EXIT_CTRL_C) break;
		err = serverInnerMenu(sockfd,username);
		if(err == EXIT_CTRL_C) break;
	}
	close(sockfd);
}

#endif
