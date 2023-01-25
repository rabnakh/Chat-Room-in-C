#ifndef CLIENT_DRIVER_H
#define CLIENT_DRIVER_H

#include "clientLoginMenu.h"
#include "clientInnerMenu.h"

void client_driver(int sockfd){
	while(1){
		clientLoginMenu(sockfd);
		clientInnerMenu(sockfd);
	}
}

#endif
