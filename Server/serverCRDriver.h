#ifndef SERVER_DRIVER_H
#define SERVER_DRIVER_H
#include "loginMenuServer.c"

void serverDriver(int sockfd){
	char option;
	while(1){
	
		// Read user option
		option = readUserOption(sockfd);	
		printf("Option: %c\n",option);

		// Create a new user
		if(option == '1'){
		}

		// Login to existing profile
		else if(option == '2'){	
		}
	}
}

#endif
