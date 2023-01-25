#ifndef ERR_HANDLE_H
#define ERR_HANDLE_H

#include <stdio.h>
#include <stdlib.h>

void error(char *msg){
	perror(msg);
	exit(1);
}

#endif
