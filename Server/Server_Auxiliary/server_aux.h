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

#endif
