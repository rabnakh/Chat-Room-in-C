#ifndef CLIENT_DRIVER_H
#define CLIENT_DRIVER_H

#include <signal.h>
#include "clientLoginMenu.h"
#include "clientInnerMenu.h"

/*
extern int sockfd;
extern struct termios initial;
*/

void signal_callback_handler(int signum){
	system("clear");
	printf("CHAT ROOM TERMINATED\n");
	tcsetattr(STDIN_FILENO,TCSANOW,&initial);
	close(sockfd);
	exit(signum);
}

void client_driver(){
	signal(SIGINT,signal_callback_handler);
	while(1){
		clientLoginMenu();
		clientInnerMenu();
	}
}

#endif
