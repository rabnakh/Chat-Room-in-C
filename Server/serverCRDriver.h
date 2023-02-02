#ifndef SERVER_DRIVER_H
#define SERVER_DRIVER_H

#include "serverLoginMenu.h"
#include "serverInnerMenu.h"

void removeClientSocket(int sockfd){
	int clientIndex = 0;
	while(users[clientIndex] != sockfd){
		clientIndex++;
	}
	while(clientIndex < usersCount){
		users[clientIndex] = users[clientIndex + 1];
		clientIndex++;
	}
	usersCount--;
}

void *serverDriver(void *arg){
	int sockfd = (long) arg;
	int userLine;
	int termCode;
	while(1){
		userLine = serverLoginMenu(sockfd);
		if(userLine == -1) break;
		termCode = serverInnerMenu(sockfd,userLine);
		if(termCode == 1) break;
	}
	removeClientSocket(sockfd);
	close(sockfd);
}

#endif
