#ifndef SERVER_AUX_H
#define SERVER_AUX_H

#include <stdio.h>
#include <unistd.h>
#include "../../Auxiliary/err_handle.h"

#define EXIT_CLEAN 0
#define EXIT_ESC 1
#define EXIT_CTRL_C 2

// Assigns the value of option
// Returns EXIT_CTRL_C if user pressed Ctrl-C
// Otherwise it returns EXIT_CLEAN
int readUserOption(int sockfd,char *option){
	int n;
	n = read(sockfd,option,sizeof(char));
	if(n == 0){
		perror("ERROR: Reading from Socket\n");
		return EXIT_CTRL_C
	}
	return EXIT_CLEAN;
}

// Return EXIT_CLEAN if ENTER pressed
// Return EXIT_ESC if ESC pressed
// Return EXIT_CTRL_C if CTRL-C
int breakCurrentAction(int sockfd){
	int err;
	int esc;
	// Read if user pressed ESC
	err = read(sockfd,&esc,sizeof(esc));
	if(err == 1){
		perror("ERROR: Reading from Socket\n");
		return EXIT_CTRL_C;
	}
	if(esc == 0){
		printf("RETURN TO LOGIN MENU\n");
		return EXIT_ESC;
	}
	return EXIT_CLEAN;
}

#endif
