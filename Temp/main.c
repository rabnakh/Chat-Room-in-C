#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "h1.h"

void signal_callback_handler(int signum){
	system("clear");
	printf("PROGRAM TERMINATED\n");
	exit(signum);
}

int main(){	
	signal(SIGINT,signal_callback_handler);
	fun1();
	return EXIT_SUCCESS;
}
