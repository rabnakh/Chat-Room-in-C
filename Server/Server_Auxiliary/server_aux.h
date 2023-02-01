#ifndef SERVER_AUX_H
#define SERVER_AUX_H

#include <stdio.h>
#include <unistd.h>
#include "../../Auxiliary/err_handle.h"

char readUserOption(int sockfd){
	int n;
	char option;
	n = read(sockfd,&option,sizeof(option));
	if(n == 0) error("ERROR: Reading to Socket\n");
	return option;
}

// Return 0 if ENTER pressed, Return 1 if ESC pressed, Return -1 if Ctrl-C
int breakToLoginMenu(int sockfd){
	int err;
	int esc;
	// Read if user pressed ESC
	err = read(sockfd,&esc,sizeof(esc));
	if(err == 1){
		perror("ERROR: Reading from Socket\n");
		return -1;
	}
	printf("esc: %d\n",esc);
	if(esc == 0){
		printf("RETURN TO LOGIN MENU\n");
		return 1;
	}
	return 0;
}

#endif
